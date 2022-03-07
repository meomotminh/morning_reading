/* --------------------------------- c file --------------------------------- */
extern int g;       // g is extern defined in .s file
int h;              // global h, used in .s file
main()
{
    int a,b,c,*bp;  // locals of main()
    g = 100;        // use g in .s file
    bp = getbp();   // call getbo() in .s file
    c = mysum(a,b); // call mysum() in .s file
    printf("a = %d b = %d c = %d\n", a,b,c);
}