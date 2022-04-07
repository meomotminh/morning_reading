/*
    Load cylinder if a cylinder is about to cross 64KB, compute NSEC=max sectors
    without crossing 64KB. Load NSEC sectors, load remaining CYL-NSEC sectors then 
    load cylinders again, etc
*/ 

#define TRK 18
#define CYL 36
int setup, ksectors, ES;
int csector = 1; // current loading sector
int NSEC = 35;  // initital number of sectors to load >= BOOT + SETUP
int getsector(u16 sector){
    readfd(sector/CYL, ((sector)%CYL)/TRK, (((sector)%CYL)%TRK));
    csector += NSEC; inces();
}
main(){
    setes(0x9000);
    getsector(1); // load linux [boot+SETUP] to 0x9000
    // current sector = SETUP's sector count (at offset 512+497) + 2
    setup = *(u8 *)(512+497)+2;
    ksectors = (*(u16 *)(512+500)) >> 5;
    NSEC = CYL - setup;     // sector remain in cylinder 0
    setes(0x1000);      // Linux kernel is loaded to segment 0x1000
    getsector(setup);   // load the remaining sectors of cylinder 0
    csector = CYL;      // we are now at beginning of cyl#1
    while (csector < ksectors + setup){
        // try to load cylinders
        ES = getes();   // current ES value
        if (((ES + CYL*0x20) & 0xF000) == (ES & 0xF000)){
            // same segment 
            NSEC = CYL; // load full cylinder
            getsector(csector); putc('C'); // show loaded a cylinder
            continue;
        }
        // this cylinder will cross 64KB, compute MAX sectors to load 
        NSEC = 1;
        while (((ES + NSEC*0x20) & 0xF000) == (ES & 0xF000)){
            NSEC++; putc('s');  // number of sectors can still load without crossing 64KB boundary
        }
        getsector(csector);
        NSEC = CYL - NSEC;  // load remaining sectors of cylinder
        putc('|');  // show cross 64KB
        getsector(csector); // load remainder of cylinder
        putc('p');
    }
}


