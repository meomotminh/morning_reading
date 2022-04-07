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
    }
}