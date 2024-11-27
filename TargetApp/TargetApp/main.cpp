#include <iostream>
#include <thread>

extern "C" __declspec(dllexport) int Calculate(int a, int b) {
	return a + b;
}

int main() {
	int result = Calculate(5, 3);
	std::cout << "Result: " << result << std::endl;

	system("pause");

	return 0;
}