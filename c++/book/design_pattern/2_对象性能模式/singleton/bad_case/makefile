example1: main.cpp hello.so
	$(CXX) $(CXXFLAGS) -std=c++11 -o example1 main.cpp -ldl -rdynamic
	# $(CXX) $(CXXFLAGS) -std=c++11 -o example1 main.cpp -ldl
hello.so: hello.cpp
	$(CXX) $(CXXFLAGS) -std=c++11 -shared -fPIC -o hello.so hello.cpp
clean:
	rm -f example1 hello.so
.PHONY: clean
