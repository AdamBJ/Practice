#include<cstdio>

int main() {
	int weight;

	scanf("%d", &weight);
	if (weight == 2) printf("NO");
	else if (weight % 2 == 0) printf("YES"); //weight is even, its possible to divide
	else printf("NO");

	return 0;
}