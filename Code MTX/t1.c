/* -------------------------------------------------------------------------- */
/*                                    t1.c                                    */
/* -------------------------------------------------------------------------- */

int g = 100;        // initialized global var
int h;              // uninitialized global var
static int s;       // static global var
main(int argc, char *argv[ ]) // main function
{
    int a = 1; int b;   // auto local var
    static int c = 3;   // static local var
    b = 2;
    c = mysum(a,b);     // call mysum(), passing a,b
    printf("sum = %d\n", c); // call printf()
}

