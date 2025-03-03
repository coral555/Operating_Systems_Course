//Submitted by: Coral Bahofrker

#include <iostream>
#include <vector>
#include <map>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;
#define DISK_SIZE 512

/*
 * Function to convert decimal to binary char
 */
char decToBinary(int n) {
    return static_cast<char>(n);
}

/*
 * Function to convert binary to decimal
 */
int binaryToDec(char ch) {
    return static_cast<int>(ch);
}

// #define SYS_CALL
// ============================================================================
class fsInode {
    int fileSize; // the current file size
    int block_in_use; // hoe many blocks are in use now

    int directBlock1; // pointer to data
    int directBlock2; // pointer to data
    int directBlock3; // pointer to data

    int singleInDirect; // pointer to pointer to data - can save until another block_size blocks
    int doubleInDirect; // pointer to pointer to pointer to data
    int block_size; // the size of the block


public:
    /*
     * Constructor
     */
    fsInode(int _block_size) {
        fileSize = 0;
        block_in_use = 0;
        block_size = _block_size;
        directBlock1 = -1;
        directBlock2 = -1;
        directBlock3 = -1;
        singleInDirect = -1;
        doubleInDirect = -1;
    }

    /*
     * return the file size
     */
    int getFileSize(){
        return this->fileSize;
    }

    /*
     * return the number of blocks that are in use now
     */
    int getNumOfBlockInUse(){
        return this->block_in_use;
    }

    /*
     * return the block size
     */
    int getBlockSize(){
        return this->block_size;
    }

    /*
     * return directBlock1
     */
    int getDirectBlock1(){
        return this->directBlock1;
    }

    /*
     * return directBlock2
     */
    int getDirectBlock2(){
        return this->directBlock2;
    }

    /*
     * return directBlock3
     */
    int getDirectBlock3(){
        return this->directBlock3;
    }

    /*
     * return singleInDirect
     */
    int getSingleInDirect(){
        return this->singleInDirect;
    }

    /*
     * return doubleInDirect
     */
    int getDoubleInDirect(){
        return this->doubleInDirect;
    }

    /*
     * set a new file size
     */
    void setFileSize(int newFileSize){
        this->fileSize += newFileSize;
    }

    /*
     * set num of blocks in use
     */
    void setBlockInUse(int num){
        this->block_in_use += num;
    }

    /*
     * set a new block size
     */
    void setBlockSize(int size){
        this->block_size = size;
    }

    /*
     * set direct block 1
     */
    void setDirectBlock1(int number){
        this->directBlock1 = number;
    }

    /*
     * set direct block 2
     */
    void setDirectBlock2(int number){
        this->directBlock2 = number;
    }

    /*
     * set direct block 3
     */
    void setDirectBlock3(int number){
        this->directBlock3 = number;
    }

    /*
     * set single block
     */
    void setSingleInDirect(int number){
        this->singleInDirect = number;
    }

    /*
     * set double block
     */
    void setDoubleInDirect(int number){
        this->doubleInDirect = number;
    }

    /*
     * return the maximum file size
     */
    int getMaxFileSize(){
        int max = (3 + block_size + block_size * block_size) * block_size;
        return max;
    }

    /*
     * return the maximum number of blocks of a file in our system
     */
    int getMaxNumOfBlocks(){
        int max = 3 + block_size + block_size * block_size;
        return max;
    }

    /*
     * return true if the block is allocated
     */
    bool isDirectBlock1Allocated(){
        if(this->getDirectBlock1() == -1){
            return false;
        }
        return true;
    }

    /*
     * return true if the block is allocated
     */
    bool isDirectBlock2Allocated(){
        if(this->getDirectBlock2() == -1){
            return false;
        }
        return true;
    }

    /*
     * return true if the block is allocated
     */
    bool isDirectBlock3Allocated(){
        if(this->getDirectBlock3() == -1){
            return false;
        }
        return true;
    }

    /*
     * return true if the block is allocated
     */
    bool isSingleBlockAllocated(){
        if(this->getSingleInDirect() == -1){
            return false;
        }
        return true;
    }

    /*
     * return true if the block is allocated
     */
    bool isDoubleBlockAllocated(){
        if(this->getDoubleInDirect() == -1){
            return false;
        }
        return true;
    }
};

// ============================================================================
class FileDescriptor {
    pair<string, fsInode*> file;
    bool inUse;

public:
    /*
     * Constructor
     */
    FileDescriptor(string FileName, fsInode* fsi) {
        file.first = FileName;
        file.second = fsi;
        inUse = true;

    }

    /*
     * return the file name
     */
    string getFileName() {
        return file.first;
    }

    /*
     * set a new file name
     */
    void setName(string name){
        if(!name.empty()){
            this->file.first = name;
        }
        else{
            cout << "ERR" <<endl;
        }
    }

    /*
     * return the inode
     */
    fsInode* getInode() {
        return file.second;
    }

    /*
     * return the file size
     */
    int GetFileSize() {
        int fileSize = 0;
        fileSize = file.second->getFileSize();
        return fileSize;
    }

    /*
     * return if the file is in use or not
     */
    bool isInUse() {
        return (inUse);
    }

    /*
     * ste the file to be in use or not
     */
    void setInUse(bool _inUse) {
        inUse = _inUse ;
    }
};

#define DISK_SIM_FILE "DISK_SIM_FILE.txt"
// ============================================================================
class fsDisk {
    FILE *sim_disk_fd; // the file itself
    bool is_formated; //  if the file was formatted

    // BitVector - "bit" (int) vector, indicate which block in the disk is free
    //              or not.  (i.e. if BitVector[0] == 1 , means that the
    //             first block is occupied.
    int BitVectorSize;
    int *BitVector;
    int BlockSize; // the block size

    // Unix directories are lists of association structures,
    // each of which contains one filename and one inode number.
    map<string, fsInode*>  MainDir ;

    // OpenFileDescriptors --  when you open a file,
    // the operating system creates an entry to represent that file
    // This entry number is the file descriptor.
    vector< FileDescriptor > OpenFileDescriptors;

public:
    // ------------------------------------------------------------------------
    /*
     * Constructor
     */
    fsDisk() {
        sim_disk_fd = fopen( DISK_SIM_FILE , "w+" ); // changed from r+ to w+
        assert(sim_disk_fd);
        for (int i=0; i < DISK_SIZE ; i++) {
            int ret_val = fseek ( sim_disk_fd , i , SEEK_SET );
            ret_val = fwrite( "\0" ,  1 , 1, sim_disk_fd );
            assert(ret_val == 1);
        }
        fflush(sim_disk_fd);
    }

    // ------------------------------------------------------------------------
    /*
     * Input: -
     * Output: A private method for a single block allocation
     */
    int blockAllocation(){
        int i;
        for(i = 0 ; i < BitVectorSize ; i++){
            if(BitVector[i] == 0){ // if the block isn't occupied
                BitVector[i] = 1; // allocating the block
                return i;
            }
        }
        cout << "ERR" << endl; // in case of an error
        return -1;
    }

    // ------------------------------------------------------------------------
    /*
     * Input: -
     * Output: A private method to check if the disk is full (true) or not
     */
    bool isDiskFull(){
        int i;
        for(i = 0 ; i < BitVectorSize ; i++){
            if(BitVector[i] == 0){ // if the block isn't occupied
                return false;
            }
        }
        return true;
    }
    // ------------------------------------------------------------------------
    /*
     * print the disk content
     */
    void listAll() {
        if(is_formated == 0){ // if the disk wasn't formatted
            cout << "ERR" << endl;
            return;
        }
        int i = 0;
        for ( auto it = begin (OpenFileDescriptors); it != end (OpenFileDescriptors); ++it) {
            cout << "index: " << i << ": FileName: " << it->getFileName() <<  " , isInUse: "
                 << it->isInUse() << " file Size: " << it->GetFileSize() << endl;
            i++;
        }
        char bufy;
        cout << "Disk content: '" ;
        for (i=0; i < DISK_SIZE ; i++) {
            int ret_val = fseek ( sim_disk_fd , i , SEEK_SET );
            ret_val = fread(  &bufy , 1 , 1, sim_disk_fd );
            cout << bufy;
        }
        cout << "'" << endl;
    }

    // ------------------------------------------------------------------------
    /*
     * Input: block size
     * Output: initialize the disk
     */
    void fsFormat( int blockSize =4) {
        if(blockSize == 1){ // if the input block size is 1
            cout << "ERR" << endl;
            return;
        }
        if(blockSize <= 0 || blockSize > DISK_SIZE){ // if illegal block size
            cout << "ERR" << endl;
            return;
        }
        this->BlockSize = blockSize; // initialize block size
        int i, j;
        // initialize the disk
        for(i = 0 ; i < DISK_SIZE ; i++){
            int ret_val = fseek ( sim_disk_fd , i , SEEK_SET );
            ret_val = fwrite( "\0" ,  1 , 1, sim_disk_fd );
            assert(ret_val == 1);
        }
        fflush(sim_disk_fd);

        // initialize bit vector
        BitVectorSize = DISK_SIZE / blockSize;
        BitVector = new int[BitVectorSize];
        for(j = 0 ; j < BitVectorSize ; j++){
            BitVector[j] = 0;
        }
        OpenFileDescriptors.clear(); // initialize OpenFileDescriptors
        MainDir.clear(); // initialize directory
        is_formated = true; // success
    }

    // ------------------------------------------------------------------------
    /*
     * Input: file name.
     * Output: file descriptor.
     */
    int CreateFile(string fileName) {
        if(is_formated == 0){ // if the disk wasn't formatted
            cout << "ERR" << endl;
            return -1;
        }
        if(fileName.empty()){ // file with no name
            cout << "ERR" << endl;
            return -1;
        }
        if(MainDir.find(fileName) != MainDir.end()){ // if the new file already exists
            cout << "ERR" << endl;
            return -1;
        }
        bool isDeleted = false;
        for(int i = 0 ; i < OpenFileDescriptors.size() ; i++) {
            if (OpenFileDescriptors[i].isInUse() == 0 && OpenFileDescriptors[i].getFileName() == " ") { // if deleted file's place
                isDeleted = true;
                auto *newInode = new fsInode(BlockSize); // create a new inode
                auto *newFd = new FileDescriptor(fileName, newInode); // create a new file descriptor
                this->MainDir[fileName] = newInode; // add to directory
                OpenFileDescriptors[i] = *newFd; // put at the free place
                return i; // return index/fd
            }
        }
        bool foundFree = false;
        if(!isDeleted){ // no free space foundFree
            for(int k = 0 ; k < OpenFileDescriptors.size() ; k++){
                if(OpenFileDescriptors[k].isInUse() == 0){
                    foundFree = true;
                    auto* newInode = new fsInode(BlockSize); // create a new inode
                    auto* newFd = new FileDescriptor(fileName,newInode); // create a new file descriptor
                    this->MainDir[fileName] = newInode; // add to directory
                    auto* oldInode = new fsInode(BlockSize); // create a new inode
                    auto* oldFd = new FileDescriptor(" ", oldInode); // create a new file descriptor
                    *oldFd = OpenFileDescriptors[k];
                    OpenFileDescriptors[k] = *newFd;
                    OpenFileDescriptors.push_back(*oldFd); // add the old to the end
                    return k;
                }
            }
        }

        if(!foundFree){
            auto* newInode = new fsInode(BlockSize); // create a new inode
            auto* newFd = new FileDescriptor(fileName,newInode); // create a new file descriptor
            this->MainDir[fileName] = newInode; // add to directory
            OpenFileDescriptors.push_back(*newFd); // add to file descriptor
            return (int) OpenFileDescriptors.size() -1; // return index/fd
        }
        cout << "ERR" << endl; // in case of error
        return -1;
    }

    // ------------------------------------------------------------------------
   /*
    * Input: file name
    * Output: file descriptor
    */
    int OpenFile(string FileName ) {
        if(is_formated == 0){ // if the disk wasn't formatted
            cout << "ERR" << endl;
            return -1;
        }
        if(MainDir.find(FileName) == MainDir.end()){ // if the new file is not exists
            cout << "ERR" << endl;
            return -1;
        }
        if(MainDir[FileName]->getBlockSize() == 1){ // if block size = 1
            cout << "ERR" << endl;
            return -1;
        }
        if(FileName.empty()){ // empty name
            cout << "ERR" << endl;
            return -1;
        }
        int i, index = -1;
        for( i = 0 ; i < OpenFileDescriptors.size() ; i++){ // get the file's location
            if(OpenFileDescriptors[i].getFileName() == FileName){
                index = i; // save its index
                break;
            }
        }
        if(index == -1){  // if file not found
            cout << "ERR" << endl;
            return -1;
        }
        if(OpenFileDescriptors[index].isInUse() == 1){ // if the file is already open
            cout << "ERR" << endl;
            return -1;
        }
        if(OpenFileDescriptors[index].isInUse() == 0){ // if the file is close
            for(int j = 0 ; j < OpenFileDescriptors.size() ; j++){
                if(OpenFileDescriptors[j].isInUse() == 0){ // if there's a free fd space
                    swap(OpenFileDescriptors[j], OpenFileDescriptors[index]); // switch places
                    OpenFileDescriptors[j].setInUse(true); // open the file at its new fd
                    return j; // return index/fd
                }
            }
        }
        else{ // if there's not a free fd space
            OpenFileDescriptors[index].setInUse(true); // open the file at its original index
            return index;
        }
        cout << "ERR" << endl; // in case of error
        return -1;
    }

    // ------------------------------------------------------------------------
    /*
     * Input: file descriptor.
     * Output: file name.
     */
    string CloseFile(int fd) {
        string name;
        if(is_formated == 0){ // if the disk wasn't formatted
            cout << "ERR" << endl;
            return "-1";
        }
        if(fd < 0){ // if illegal fd
            cout << "ERR" << endl;
            return "-1";
        }
        if( fd >= OpenFileDescriptors.size()){ // if illegal fd
            cout << "ERR" << endl;
            return "-1";
        }
        if(OpenFileDescriptors[fd].isInUse() == 0){ // if the file is already close
            cout << "ERR" << endl;
            return "-1";
        }
        if(OpenFileDescriptors[fd].getInode()->getBlockSize() == 1){ // if block size = 1
            cout << "ERR" << endl;
            return "-1";
        }
        name = OpenFileDescriptors[fd].getFileName(); // save the file name in order to return it
        OpenFileDescriptors[fd].setInUse(false); // close the file
        return name;
    }
    // ------------------------------------------------------------------------
    /*
     * Input: file descriptor, text to write, length.
     * Output: 1 for success, else -1.
     */
    int WriteToFile(int fd, char *buf, int len ) {
        if(is_formated == 0){ // if the disk wasn't formatted
            cout << "ERR" << endl;
            return -1;
        }
        if(fd < 0){ // if illegal fd
            cout << "ERR" << endl;
            return -1;
        }
        if( fd >= OpenFileDescriptors.size()){ // if illegal fd
            cout << "ERR" << endl;
            return -1;
        }
        if(OpenFileDescriptors[fd].isInUse() == 0){ // if the file is already close
            cout << "ERR" << endl;
            return -1;
        }
        if(OpenFileDescriptors[fd].getInode()->getBlockSize() == 1){ // if the block size = 1
            cout << "ERR" << endl;
            return -1;
        }
        if(isDiskFull()){ // if the disk is full
            cout << "ERR" << endl;
            return -1;
        }
        fsInode* inode = OpenFileDescriptors[fd].getInode();
        int singleBlock = 3 + BlockSize; // single size
        int doubleBlock = 3 + BlockSize + BlockSize * BlockSize; // double size
        char search, search1;
        int find, find2;
        for(int i = 0 ; i < len ; i++){
            int offset = inode->getFileSize() % BlockSize; // calculate offset
            int location = inode->getFileSize() / BlockSize; // calculate location in file
            if(location < 1){ // if we're in direct block 1
                if(!inode->isDirectBlock1Allocated()){ // if direct block 1 isn't allocated
                    inode->setDirectBlock1(blockAllocation()); // allocate it
                }
                fseek(sim_disk_fd,inode->getDirectBlock1() * BlockSize + offset, SEEK_SET); // find the position to write the data at the block
                fwrite(&buf[i], 1, 1, sim_disk_fd); // write the data
                inode->setFileSize(1); // update file's size
            }
            else if(location < 2){ // if we're in direct block 2
                if(!inode->isDirectBlock2Allocated()){ // if direct block 2 isn't allocated
                    inode->setDirectBlock2(blockAllocation()); // allocate it
                }
                fseek(sim_disk_fd,inode->getDirectBlock2() * BlockSize + offset, SEEK_SET); // find the position to write the data at the block
                fwrite(&buf[i], 1, 1, sim_disk_fd); // write the data
                inode->setFileSize(1); // update file's size
            }
            else if(location < 3){ // if we're in direct block 3
                if(!inode->isDirectBlock3Allocated()){ // if direct block 3 isn't allocated
                    inode->setDirectBlock3(blockAllocation()); // allocate it
                }
                fseek(sim_disk_fd,inode->getDirectBlock3() * BlockSize + offset, SEEK_SET); // find the position to write the data at the block
                fwrite(&buf[i], 1, 1, sim_disk_fd); // write the data
                inode->setFileSize(1); // update file's size
            }
            else if(location < singleBlock){ // if we're in single in direct block
                if(!inode->isSingleBlockAllocated()){ // if direct block 3 isn't allocated
                    inode->setSingleInDirect(blockAllocation()); // allocate it
                }
                find = (inode->getFileSize()-3 * BlockSize) / BlockSize; // where is my location now, from where to continue
                fseek(sim_disk_fd, inode->getSingleInDirect() * BlockSize + find, SEEK_SET); // from which char to continue
                fread(&search, 1, 1, sim_disk_fd); // read 1 byte to search in order to go to its place
                if(search == 0){ // if not allocated
                    search = decToBinary(blockAllocation()); // allocate it
                    fseek(sim_disk_fd, inode->getSingleInDirect()*BlockSize + find, SEEK_SET); // make sure it's at the same place
                    fwrite(&search, 1, 1, sim_disk_fd); // write 1 data byte
                }
                fseek(sim_disk_fd, binaryToDec(search) * BlockSize + offset, SEEK_SET); // move to where you need to write the data
                fwrite(&buf[i], 1, 1, sim_disk_fd); // write the data
                inode->setFileSize(1); // update file's size
            }
            else if(location < doubleBlock){ // if we're in double in direct block
                if(!inode->isDoubleBlockAllocated()){ // if not allocated
                    inode->setDoubleInDirect(blockAllocation()); // allocate it
                }
                find = (inode->getFileSize() -(3 + BlockSize) * BlockSize) / (BlockSize * BlockSize); // first pointer - block index in the pointer
                find2 = ((inode->getFileSize() -(3+BlockSize) * BlockSize) % (BlockSize*BlockSize)) / BlockSize; // second pointer - pointer to pointer - in which block we are
                fseek(sim_disk_fd, inode->getDoubleInDirect() * BlockSize + find, SEEK_SET); // go to the place of which char to choose
                fread(&search, 1, 1, sim_disk_fd); // read 1 byte to search in order to go to its place
                if(search == 0){ // if not allocated
                    search = decToBinary(blockAllocation()); // allocate it
                    fseek(sim_disk_fd, inode->getDoubleInDirect()*BlockSize + find, SEEK_SET); // make sure it's at the same place
                    fwrite(&search, 1, 1, sim_disk_fd);
                }
                fseek(sim_disk_fd, binaryToDec(search) * BlockSize + find2, SEEK_SET); // find the place i was sent to
                fread(&search1, 1, 1, sim_disk_fd); // read 1 byte to search in order to go to its place
                if(search1 == 0){ // if not allocated
                    search1 = decToBinary(blockAllocation()); // allocate it
                    fseek(sim_disk_fd, binaryToDec(search) * BlockSize + find2, SEEK_SET); // make sure it's at the same place
                    fwrite(&search1, 1, 1, sim_disk_fd);
                }
                fseek(sim_disk_fd, binaryToDec(search1) * BlockSize + offset, SEEK_SET); // move to where you need to write the data
                fwrite(&buf[i], 1, 1, sim_disk_fd); // write the data
                inode->setFileSize(1); // update file's size
            }
            fflush(sim_disk_fd);
        }
        return 1; // success
    }
    // ------------------------------------------------------------------------
    /*
     * Input: file name.
     * Output: 1 for success, else -1.
     */
    int DelFile( string FileName ) {
        if(is_formated == 0){ // if the disk wasn't formatted
            cout << "ERR" << endl;
            return -1;
        }
        if(MainDir.find(FileName) == MainDir.end()){ //  if the file isn't exists
            cout << "ERR" << endl;
            return -1;
        }
        if(MainDir[FileName]->getBlockSize() == 1){ // if block size = 1
            cout << "ERR" << endl;
            return -1;
        }
        int index = -1, i;
        for(i = 0 ; i < OpenFileDescriptors.size() ; i++){
            if(OpenFileDescriptors[i].getFileName() == FileName){ // find the file's location
                index = i; // save its index
                break;
            }
        }
        if(index == -1){ // not found
            cout << "ERR" << endl;
            return -1;
        }
        if(OpenFileDescriptors[index].isInUse() == 1){ //  if the file is open
            cout << "ERR" << endl;
            return -1;
        }
        fsInode* inode = OpenFileDescriptors[index].getInode();
        int len = inode->getFileSize();
        int singleBlock = 3 + BlockSize; // size of single in direct block
        int doubleBlock = 3 + BlockSize + BlockSize * BlockSize; // size of double in direct block
        char search, search1;
        int find, find2;

        for( i = len-1 ; i >= 0 ; i--){ // start from the end
            int offset = i % BlockSize; // calculate offset
            int location = i / BlockSize; // find location in file
            if(location < 1){ // if we're in direct block 1
                fseek(sim_disk_fd,inode->getDirectBlock1() * BlockSize + offset, SEEK_SET); // go to/find this location/position in direct block 1
                inode->setFileSize(-1); // update file's size
                BitVector[inode->getDirectBlock1()] = 0;  // update direct block 1
            }
            else if(location < 2){ // if we're in direct block 2
                fseek(sim_disk_fd,inode->getDirectBlock2() * BlockSize + offset, SEEK_SET); // go to/find this location/position in direct block 2
                inode->setFileSize(-1);  // update file's size
                BitVector[inode->getDirectBlock2()] = 0;  // update direct block 2
            }
            else if(location < 3){ // if we're in direct block 3
                fseek(sim_disk_fd,inode->getDirectBlock3() * BlockSize + offset, SEEK_SET); // go to/find this location/position in direct block 3
                inode->setFileSize(-1);  // update file's size
                BitVector[inode->getDirectBlock3()] = 0; // update direct block 3
            }
            else if(location < singleBlock){ // if we're in single in direct block
                find = (i - 3 * BlockSize) / BlockSize; // single in direct pointer
                fseek(sim_disk_fd, inode->getSingleInDirect() * BlockSize + find, SEEK_SET); // go to the pointer's location
                fread(&search, 1, 1, sim_disk_fd); // read 1 byte
                fseek(sim_disk_fd, binaryToDec(search) * BlockSize + offset, SEEK_SET); // convert the byte you read and go to/find this location/position in single in direct block
                inode->setFileSize(-1);  // update file's size
                BitVector[inode->getSingleInDirect()] = 0; // update single block
                BitVector[binaryToDec(search)] = 0; // update pointer
            }
            else if(location < doubleBlock){ // if we're in double in direct block
                find = (i -(3 + BlockSize) * BlockSize) / (BlockSize * BlockSize); // double pointer index
                find2 = ((i -(3+BlockSize) * BlockSize) % (BlockSize*BlockSize)) / BlockSize; // location inside the pointer block
                fseek(sim_disk_fd, inode->getDoubleInDirect() * BlockSize + find, SEEK_SET); // move to pointer location
                fread(&search, 1, 1, sim_disk_fd); // read 1 byte

                fseek(sim_disk_fd, binaryToDec(search) * BlockSize + find2, SEEK_SET); // convert the value and move to its place
                fread(&search1, 1, 1, sim_disk_fd); // read 1 byte

                fseek(sim_disk_fd, binaryToDec(search1) * BlockSize + offset, SEEK_SET); // convert the byte you read and go to/find this location/position in double in direct block
                inode->setFileSize(-1); // update file's size
                BitVector[inode->getDoubleInDirect()] = 0; // update double block
                BitVector[binaryToDec(search)] = 0; // update pointer
                BitVector[binaryToDec(search1)] = 0; // update pointer
            }
            fflush(sim_disk_fd);
        }

        // initialize file's blocks not to be allocated
        inode->setDirectBlock1(-1);
        inode->setDirectBlock2(-1);
        inode->setDirectBlock3(-1);
        inode->setSingleInDirect(-1);
        inode->setDoubleInDirect(-1);

        MainDir.erase(FileName); // delete from directory
        OpenFileDescriptors[index].setName(" "); // set name to an empty string
        return 1; // success
    }
    // ------------------------------------------------------------------------
    /*
     * Input: file descriptor, text to write, length.
     * Output: 1 for success, else -1.
     */
    int ReadFromFile(int fd, char *buf, int len ) {
        if(is_formated == 0){ // if the disk wasn't formatted
            cout << "ERR" << endl;
            return -1;
        }
        if(fd < 0){ // if illegal fd
            cout << "ERR" << endl;
            return -1;
        }
        if(fd >= OpenFileDescriptors.size()){ // if illegal fd
            cout << "ERR" << endl;
            return -1;
        }
        if(OpenFileDescriptors[fd].isInUse() == 0){ // if the file is already close
            cout << "ERR" << endl;
            return -1;
        }
        if(OpenFileDescriptors[fd].GetFileSize() == 0){ // empty file
            return 1;
        }
        if(OpenFileDescriptors[fd].getInode()->getBlockSize() == 1){ // if the block size = 1
            cout << "ERR" << endl;
            return -1;
        }
        fsInode* inode = OpenFileDescriptors[fd].getInode();
        int singleBlock = 3 + BlockSize; // size of single in direct block
        int doubleBlock = 3 + BlockSize + BlockSize * BlockSize; // size of double in direct block
        char search, search1;
        int find, find2;
        for(int i = 0 ; i < len ; i++){
            int offset = i % BlockSize; // calculate offset
            int location = i / BlockSize; // calculate location in file
            if(location < 1){ // if we're in direct block 1
                fseek(sim_disk_fd,inode->getDirectBlock1() * BlockSize + offset, SEEK_SET); // move to the location in the block
                fread(&buf[i], 1, 1, sim_disk_fd); // read 1 byte
            }
            else if(location < 2){ // if we're in direct block 2
                fseek(sim_disk_fd,inode->getDirectBlock2() * BlockSize + offset, SEEK_SET); // move to the location in the block
                fread(&buf[i], 1, 1, sim_disk_fd); // read 1 byte
            }
            else if(location < 3){ // if we're in direct block 3
                fseek(sim_disk_fd,inode->getDirectBlock3() * BlockSize + offset, SEEK_SET); // move to the location in the block
                fread(&buf[i], 1, 1, sim_disk_fd); // read 1 byte
            }
            else if(location < singleBlock){ // if we're in single in direct block
                find = (i-3 * BlockSize) / BlockSize; // block index where the pointer points
                fseek(sim_disk_fd, inode->getSingleInDirect() * BlockSize + find, SEEK_SET); // move to this location
                fread(&search, 1, 1, sim_disk_fd); // read 1 byte
                fseek(sim_disk_fd, binaryToDec(search) * BlockSize + offset, SEEK_SET); // convert & move to this location
                fread(&buf[i], 1, 1, sim_disk_fd); // read 1 byte
            }
            else if(location < doubleBlock){ // if we're in double in direct block
                find = (i -(3 + BlockSize) * BlockSize) / (BlockSize * BlockSize); // first pointer index
                find2 = ((i -(3+BlockSize) * BlockSize) % (BlockSize*BlockSize)) / BlockSize; // second pointer index
                fseek(sim_disk_fd, inode->getDoubleInDirect() * BlockSize + find, SEEK_SET); // go to the location of the first pointer points
                fread(&search, 1, 1, sim_disk_fd); // read 1 byte

                fseek(sim_disk_fd, binaryToDec(search) * BlockSize + find2, SEEK_SET); // convert & go to the location of the second pointer points
                fread(&search1, 1, 1, sim_disk_fd); // read 1 byte

                fseek(sim_disk_fd, binaryToDec(search1) * BlockSize + offset, SEEK_SET); // convert & go to the location of the data we want to read from the disk
                fread(&buf[i], 1, 1, sim_disk_fd); // read 1 byte
            }
        }
        buf[len] = '\0';
        return 1;
    }

    // ------------------------------------------------------------------------
    /*
     * Input: file descriptor.
     * Output: file size.
     */
    int GetFileSize(int fd) {
        int fileSize;
        if(is_formated == 0){ // if the disk wasn't formatted
            cout << "ERR" << endl;
            return -1;
        }
        if(fd < 0){ // illegal fd
            cout << "ERR" << endl;
            return -1;
        }
        if(fd >= OpenFileDescriptors.size()){ // illegal fd
            cout << "ERR" << endl;
            return -1;
        }
        fileSize = OpenFileDescriptors[fd].GetFileSize(); // get the file's size
        return fileSize;
    }

    // ------------------------------------------------------------------------
    /*
     * Input: source file name, destination file name.
     * Output: 1 for success, else -1.
     */
    int CopyFile(string srcFileName, string destFileName) {
        if(is_formated == 0){ // if the disk wasn't formatted
            cout << "ERR" << endl;
            return -1;
        }
        if(MainDir.find(srcFileName) == MainDir.end()){ // if the src file isn't exists
            cout << "ERR" << endl;
            return -1;
        }
        if(MainDir[srcFileName]->getBlockSize() == 1){ // if block size = 1
            cout << "ERR" << endl;
            return -1;
        }
        if(srcFileName == destFileName){ // if the names are equal
            cout << "ERR" << endl;
            return -1;
        }
//        if(destFileName.empty() || destFileName.size() == 0 || destFileName == " "){ // if we want to copy an un-named file
//            cout << "ERR" << endl;
//            return -1;
//        }
        int i;
        int srcIndex = -1,destIndex = -1;
        int srcSize;
        char str_to_copy[DISK_SIZE];
        for(i = 0 ; i < OpenFileDescriptors.size() ; i++){
            if(OpenFileDescriptors[i].getFileName() == srcFileName && OpenFileDescriptors[i].isInUse() == 1){ // if src file is open
                cout << "ERR" << endl;
                return -1;
            }
            if(OpenFileDescriptors[i].getFileName() == srcFileName && OpenFileDescriptors[i].isInUse() == 0){ // if src file is close
                srcIndex = i; // keep its index
                srcSize = OpenFileDescriptors[i].GetFileSize(); // keep its size
                break;
            }
        }
        bool override = false;
        if(MainDir.find(destFileName) != MainDir.end()){ // if dest file exists
            override = true; // need to be overridden
        }
        if(override){ // if the dest file need to be overridden
            for(int j = 0 ; j < OpenFileDescriptors.size() ; j++){
                if(OpenFileDescriptors[j].getFileName() == destFileName && OpenFileDescriptors[j].isInUse() == 1){ // if dest file is open
                    cout << "ERR" << endl;
                    return -1;
                }
                if(OpenFileDescriptors[j].getFileName() == destFileName && OpenFileDescriptors[j].isInUse() == 0){ // if dest file is close
                    destIndex = j; // keep it index
                    break;
                }
            }
            OpenFile(srcFileName); // open src file
            OpenFile(destFileName); // open dest file

            ReadFromFile(srcIndex, str_to_copy, srcSize); // read the data from the src file
            WriteToFile(destIndex, str_to_copy, srcSize); // write the data we read to the dest file

            CloseFile(srcIndex); // close src file
            CloseFile(destIndex); // close dest file
            return 1; // success
        }

        else if(!override){ // if not - doesn't exist
            CreateFile(destFileName); // create a new dest file
            int newIndex = -1;
            for(int k = 0 ; k < OpenFileDescriptors.size() ; k++){
                if(OpenFileDescriptors[k].getFileName() == destFileName && OpenFileDescriptors[k].isInUse() == 1){ // find the dest file location
                    newIndex = k; // keep its location
                    break;
                }
            }
            OpenFile(srcFileName); // open src file
            ReadFromFile(srcIndex,str_to_copy,srcSize); // read the data from the src file
            WriteToFile(newIndex,str_to_copy,srcSize); // write the data we read to the dest file
            CloseFile(newIndex); // close src file
            CloseFile(srcIndex); // close dest file
            return 1; // success
        }
        cout << "ERR" << endl; // in case of error
        return -1;
    }

    // ------------------------------------------------------------------------
    /*
     * Input: old file name, new file name.
     * Output: 1 foe success, else -1.
     */
    int RenameFile(string oldFileName, string newFileName) {
        int i;
        if(is_formated == 0){ // if the disk wasn't formatted
            cout << "ERR" << endl;
            return -1;
        }
        if(MainDir.find(oldFileName) == MainDir.end()){ // if the old file is not exists
            cout << "ERR" << endl;
            return -1;
        }
        if(MainDir.find(newFileName) != MainDir.end()){ // if the new file is already exists
            cout << "ERR" << endl;
            return -1;
        }
        if(MainDir[oldFileName]->getBlockSize() == 1){ // if block size = 1
            cout << "ERR" << endl;
            return -1;
        }
        int index = -1;
        for(i = 0 ; i < OpenFileDescriptors.size() ; i++){
            if(OpenFileDescriptors[i].getFileName() == oldFileName){ // find the old file location
                index = i; // keep its location
                break;
            }
        }
        if(index == -1){ // not found
            cout << "ERR" << endl;
            return -1;
        }
        if(OpenFileDescriptors[index].isInUse() == 1){ // if the file is open
            cout << "ERR" << endl;
            return -1;
        }
        if(OpenFileDescriptors[index].isInUse() == 0){ // if the file is close
            fsInode* old = MainDir[oldFileName]; // save the old name
            MainDir.erase(oldFileName); // delete the file from direcctory
            MainDir[newFileName] = old; // put in the new file name
            for(auto & OpenFileDescriptor : OpenFileDescriptors){
                if(OpenFileDescriptor.getFileName() == oldFileName){ // set the new file name
                    OpenFileDescriptor.setName(newFileName);
                    break;
                }
            }
            return 1;
        }
        cout << "ERR" << endl; // in case of error
        return -1;
    }

    // ------------------------------------------------------------------------
    /*
     * Destructor
     */
    ~fsDisk(){
        if(sim_disk_fd != nullptr){
            fclose(sim_disk_fd); // close the file
        }
        delete[] BitVector; // delete/free bitVector
        for(auto & OpenFileDescriptor :OpenFileDescriptors){
           delete OpenFileDescriptor.getInode(); // delete each inode
        }
        OpenFileDescriptors.clear(); // initialize OpenFileDescriptors
        MainDir.clear(); // initialize directory
    }
};

int main() {
    int blockSize;
    string fileName;
    string fileName2;
    char str_to_write[DISK_SIZE];
    char str_to_read[DISK_SIZE];
    int size_to_read;
    int _fd;

    fsDisk *fs = new fsDisk();
    int cmd_;
    while(1) {
        cin >> cmd_;
        switch (cmd_)
        {
            case 0:   // exit
                delete fs;
                exit(0);
                break;

            case 1:  // list-file
                fs->listAll();
                break;

            case 2:    // format
                cin >> blockSize;
                fs->fsFormat(blockSize);
                break;

            case 3:    // creat-file
                cin >> fileName;
                _fd = fs->CreateFile(fileName);
                cout << "CreateFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 4:  // open-file
                cin >> fileName;
                _fd = fs->OpenFile(fileName);
                cout << "OpenFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 5:  // close-file
                cin >> _fd;
                fileName = fs->CloseFile(_fd);
                cout << "CloseFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 6:   // write-file
                cin >> _fd;
                cin >> str_to_write;
                fs->WriteToFile( _fd , str_to_write , strlen(str_to_write) );
                break;

            case 7:    // read-file
                cin >> _fd;
                cin >> size_to_read ;
                fs->ReadFromFile( _fd , str_to_read , size_to_read );
                cout << "ReadFromFile: " << str_to_read << endl;
                break;

            case 8:   // delete file
                cin >> fileName;
                _fd = fs->DelFile(fileName);
                cout << "DeletedFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 9:   // copy file
                cin >> fileName;
                cin >> fileName2;
                fs->CopyFile(fileName, fileName2);
                break;

            case 10:  // rename file
                cin >> fileName;
                cin >> fileName2;
                fs->RenameFile(fileName, fileName2);
                break;

            default:
                break;
        }
    }

}