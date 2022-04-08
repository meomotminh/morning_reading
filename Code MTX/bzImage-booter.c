/* --------------------- C code of Linux bzImage booter --------------------- */
u16 iblock, NSEC = 2;
char b1[1024], b2[1024], b3[1024];  // b2[] and b3[] are adjacent
main(){
    char *cp, *name[2];
    u16  i, ino, setup, blk, nblk;
    u32  *up;
    INODE *ip;
    GD  *gp;
    name[0] = "boot"; name[1] = "linux"; // hard coded /boot/linux
    getblk(2, b1);
    gp = (GD *)b1; // get group0 descriptor to find inode table start block
    // read inode start block to get root inode
    iblock = (u16)gp->bg_inode_table;
    getblk(iblock, b1);
    ip = (INODE *)b1 + 1;   // ip points at root inode
    // search for image file name
    for (ip = 0; i<2; i++){
        ino = search(ip, name[i]) - 1;
        if (ino < 0) error();
        getblk(iblk + (ino/8), buf1);   // read inode block of ino
        ip = (INODE *)buf1 + (ino % 8);
    }

    getblk((u16)ip->i_block[0], b2);
    setup = b2[497];
    nblk = (1+setup)/2; // number of [bootsector + SETUP] blocks
    // read in indirect & double indirect blocks before changing ES
    getblk((u16)ip->i_block[12], b2); // get indirect block into b2[]
    getblk((u16)ip->i_block[13], b3); // get db indirect block into b3[]
    up = (u32 *)b3;
    getblk((u16)*up, b3); // get first double indirect into b3
    setes(0x9000);  // loading segment of BOOT + SETUP
    for (i = 0; i<12; i++){
        if (i == nblk){
            if ((setup & 1) == 0) // if setp=even -> need 1/2 block more
                getblk((u16)ip->i_block[i], 0);
                setes(0x1000);  // Set ES for kernel image at 0x1000
        }
        getblk((u16)ip->i_block[i], 0); // setup = event:1/2 SETUP
        inces();
    }
    // load indirect and double indirect blocks in b2[]b3[]
    up = (u32 *)b2; // access b2[]b3[] as u32
    while (*up++){
        getblk((u16)*up, 0);    // load block to (ES, 0)
        inces(); putc('.');
    }
    // finally if setup is even, move kernel image DOWN 1 sector
    if ((setup & 1)==0)
        for (i = 1; i<9; i++)
            move(i*0x1000); // move one 64KB segment at a time

}