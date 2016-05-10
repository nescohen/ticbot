#include <stdio.h>

int main()
{
	char buffer[100];
	char word[50];
	int count;
	fgets(buffer, 100, stdin);
	count = sscanf(buffer, "%s", word);
	while (count != EOF && count > 0)
	{
		printf("%s\n", word);
		count = sscanf(buffer, "%s", word);
	}
	return 0;
}