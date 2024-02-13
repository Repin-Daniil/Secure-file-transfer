FROM gcc:11.3 as build

RUN apt update && \
    apt install -y \
      python3-pip \
      cmake \
    && \
    pip3 install conan==1.*

COPY conanfile.txt /app/
RUN mkdir /app/build && cd /app/build && \
        conan install .. --build=missing -s build_type=Release -s compiler.libcxx=libstdc++11

COPY ./src /app/src
COPY ./tests /app/tests
COPY ./res /app/res
COPY CMakeLists.txt /app/

RUN cd /app/build && \
        cmake .. -DCMAKE_BUILD_TYPE=Debug && \
        cmake --build . --target all

FROM ubuntu:22.04 as run

COPY --from=build /app/build/bin/secure_file_transfer /app/bin/
COPY --from=build /app/build/bin/smoke_test /app/bin/
COPY ./res /app/res

ENTRYPOINT ["bash", "-c", "cd /app/bin && exec bash"]
