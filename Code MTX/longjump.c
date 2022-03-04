/* ---------------------- demostrate long jump in linux --------------------- */

#include <stdio.h>
#include <setjmp.h>

jmp_buf env;        // for saving longjmp env
main()
{
    int r, a = 100;
    printf("call setjmp to save environment\n");
    if ((r=setjmp(env)) == 0){
        A();
        printf("normal return\n");
    }
    else 
        printf("back to main() via long jump, r= %d a=%d\n", r, a);
}

int A()
{
    printf("Enter A()\n");
    B();
    printf("Exit A()\n");
}

int B()
{
    printf("Enter B()\n");
    printf("long jump? (y|n) ");
    if (getchar() == 'y')
        longjmp(env, 1234);
    printf("exit B()\n");
}