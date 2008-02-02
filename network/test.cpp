#include "Network.h"

int main()
{
	Network n = Network();

	int kaka = n.connect("127.0.0.1", 1234);

	printf("baba: %d\n", kaka);
}
