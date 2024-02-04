constexpr const char* hello = "Hello from C++";

extern "C" {

const char* __attribute__((__cdecl__)) getHello() {
  return hello;
}

}
