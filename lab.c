int global = 5;

int foo1 (int x){
    int x;
    return x + 5;
}

void foo2() {
    foo1(2);
    printf("5\n");
}

char* foo3 (char *c, int d)
{
    int d;
    char c;
    int global = 3 + 1;
    return c;
}

float d = 3.31 + (21);

char* foo4(char *c)
{
    int d = 5;
    return c;
}

char *foo5(char *c)
{
    return c;
}

char *foo6(char *c, int d)
{
    return c;
}

char *foo7(char *c)
{
    int i;

    for (int i = 0; i < 0; i++) {
        for (int i = 0; i < 3; i--) {
            continue;
        }

        int i;
    }

    int i;

    return c;
}

char *foo8(char *c)
{
    return c;
    foo7(c);
}

int main()
{
    foo1(3);
    return 0;
}
