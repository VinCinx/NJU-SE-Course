#define INEFFECTIVE_COMMAND 0
#define myexit 1
#define ls 2
#define lsWithOptionL 3
#define cat 4

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <string.h>//memcpy
#include <stdio.h>//fopen等
#include <stdlib.h>//调用exit()
using namespace std;
//print函数只写一个char*的 string转化为char* int和char都通过ostringstream转化为string 然后再进行处理
extern "C" {
    void asm_print(const char*);
}

const int IMAGE_SIZE = 1440 * 1024;//1474560
char IMG[IMAGE_SIZE]={0};//通过IMG来存放软盘上的数据，之后的读取都基于IMG，无需再对.img文件进行读取  用0初始化，而不是‘0’
/*
    BPB（BIOS Paramter Block）,按照先后顺序定义变量，省略用不到的变量
    为了便于处理，直接使用全局变量
*/
short BPB_BytsPerSec; // *每扇区字节数
char BPB_SecPerClus;  // *每簇占用的扇区数
short BPB_RsvdSecCnt; // *Boot占用的扇区数
char BPB_NumFATs;     // **FAT表的个数
short BPB_RootEntCnt; // **根目录最大文件数
short BPB_TotSec16;   // **扇区总数 0xB40=2880
short BPB_FATSz16;    // **每个FAT占用扇区数 0x9=9
short BPB_SecPerTrk;  // 每个磁道扇区数
int BPB_HiddSec;      // 隐藏扇区数
int BPB_TotSec32;     // 如果BPB_TotSec16是0，则在这里记录
char BS_FileSysType[8];// 文件系统类型，必须是8个字符，不足填充空格
//几个基址
int Fat1Base;
int RootDirBase;
int DataBase;
/*
    根目录区由目录项构成
    下面是目录项的数据结构
    定义为结构体，更加便于数据的访问
*/
struct Entry
{
    char DIR_Name[11];
    char DIR_Attr;
    char reserve[10];
    short DIR_WrtTime;
    short DIR_WrtDate;
    char DIR_FstClus[2];//*
    char DIR_FileSize[4];//int
};
/*
下面定义几个方法
*/
void initializeIMG(const char* imgPath);
void initializeBPB();
void virtualDiskHead (char* re, int secNum, int begin, int size);//虚拟磁头，第几个扇区，begin和size指明读取512字节中的哪些字节
int char2int (const char* theChars, int begin, int size);//连续的char读取出来，转化为int
void invalidImg();
void redStart();
void redEnd();
//为输出服务的几个方法
void redStart() {
    asm_print("\033[31m");
}
void redEnd() {
    asm_print("\033[0m");
}
string int2String(int i){
    ostringstream os;
    os << i;
    return os.str();
}
string char2String(char c){
    ostringstream os;
    os << c;
    return os.str();
}

int commandParser(const string s, vector<string>* const args );
void parsePath(string userPath,vector<string>* paths);
int fatTableReader(int clusNum);
string DirName2RealName(Entry *entry);

void printRootEntries(int type);
void dfsPrint(Entry entry, vector<string>* pathBeforeName,int type);//非根目录调用得打印函数
void printCount(Entry entry);//根目录调用的打印函数
void printRootCount();

void visitRelativeRoot(Entry entry, vector<string>* pathBeforeName, int type);

void printRootEntries(int type){
    if(type==ls)
        asm_print("/:\n");
    else{
        asm_print("/  ");
        printRootCount();
        asm_print(":\n");
    }

/*
打印根目录里的目录数和文件数
*/
    int entryNum=0;
    while(entryNum<BPB_RootEntCnt){
        char anEntry[32];
        virtualDiskHead(anEntry, RootDirBase, entryNum*32, 32);
        Entry loadEntry;
        memcpy(&loadEntry, anEntry, 32);//将数据结构化
        int dirAttr=(unsigned char)(loadEntry.DIR_Attr);
        string toPrint="";

        int char0=char2int(loadEntry.DIR_Name,0,1);
        // if(char0==229)
        //     continue;

        if( (dirAttr==16 || dirAttr==32) && char0!=229){
            if(type==ls){
                if(dirAttr==16){
                    toPrint=DirName2RealName(&loadEntry);
                    redStart();
                    asm_print(const_cast<char*>(toPrint.c_str()));
                    redEnd();
                    asm_print("  ");
                }else{
                    toPrint=DirName2RealName(&loadEntry);
                    asm_print(const_cast<char*>(toPrint.c_str()));//打印根目录文件名
                    asm_print("  ");
                }

            }
            else{
                if(dirAttr==16){
                    toPrint=DirName2RealName(&loadEntry);
                    redStart();
                    asm_print(const_cast<char*>(toPrint.c_str()));//打印根目录文件名
                    redEnd();
                    asm_print("  ");
                    printCount(loadEntry);
                    asm_print("\n");
                }else{
                    toPrint=DirName2RealName(&loadEntry);
                    asm_print(const_cast<char*>(toPrint.c_str()));
                    asm_print("  ");//打印根目录文件名
                    int fileSize=char2int(loadEntry.DIR_FileSize,0,4);
                    asm_print(int2String(fileSize).c_str());
                    asm_print("\n");
                }

            }
            
        }else{
            ;
        }
        entryNum++;
    }
    asm_print("\n");

    entryNum=0;
    vector<string> pathBeforeName;//这里不要用指针！  vector<> *一般不用，会出错！！！
    while(entryNum<BPB_RootEntCnt){
        char anEntry[32];
        virtualDiskHead(anEntry, RootDirBase, entryNum*32, 32);
        Entry loadEntry;
        memcpy(&loadEntry, anEntry, 32);//将数据结构化
        int dirAttr=(unsigned char)(loadEntry.DIR_Attr);
        string toPrint="";
        if(dirAttr==16 || dirAttr==32){
            dfsPrint(loadEntry,&pathBeforeName,type);
        }else{
            ;
        }
        entryNum++;
    }
}


typedef struct FindEntryRe{
    bool rightPath;
    Entry entry;
}EntryResult;
//如果想要根目录的输出和非根目录输出的处理过程一样，就要特定的为了根目录伪装一个entry
//而且读取这个伪装的entry的时候，读取方法也和一般的目录entry读取方式不一样
//比较麻烦，不如直接特判
EntryResult dfsFindEntry(vector<string>* paths, Entry entry);
EntryResult findEntry(vector<string>* paths);

//之所以有两个findEntry是因为根目录是没有entry的，需要特殊处理
EntryResult findEntry(vector<string>* paths){
    FindEntryRe re;
    re.rightPath=false;// /a 根目录没有查找到/a的目录时，返回false 貌似有没有这一行，这样输入都不会出错，默认false？
    if((*paths).size()==0){
        re.rightPath=true;
        (*paths).push_back("theRootDirectory");
    }else{
        int entryNum=0;
        while(entryNum<BPB_RootEntCnt){
            char anEntry[32];
            virtualDiskHead(anEntry, RootDirBase, entryNum*32, 32);
            Entry loadEntry;
            memcpy(&loadEntry, anEntry, 32);//将数据结构化
            int dirAttr=(unsigned char)(loadEntry.DIR_Attr);
            string toPrint="";
            if(dirAttr==16 || dirAttr==32){
                string theName=DirName2RealName(&loadEntry);
                if(theName==(*paths)[0]){
                    vector<string>::iterator k=(*paths).begin();
                    (*paths).erase(k);//删除第一个元素
                    EntryResult dfsRe=dfsFindEntry(&(*paths), loadEntry);//函数调用！！！！！！！！，没写时ls /NJU/CS 这样的会有问题
                    re.entry=dfsRe.entry;
                    re.rightPath=dfsRe.rightPath;
                    if(dfsRe.rightPath)
                        break;//找到了就跳出循环！！！！！！！！！,不循环也可以，会浪费一些时间     
                }      
            }else{
                ;
            }
            entryNum++;
        }
    }
    return re;
}
EntryResult dfsFindEntry(vector<string>* paths, Entry entry){
    EntryResult re;

    if((*paths).size()!=0){
        int clusNum=char2int(entry.DIR_FstClus, 0, 2);

        while(clusNum>0){
            char dataBlock[512];//硬编码
            virtualDiskHead(dataBlock, DataBase+clusNum-2, 0, 512);
            clusNum=fatTableReader(clusNum);
            for (int i = 0; i < BPB_BytsPerSec / sizeof(Entry); i++) {
                    Entry insideEntry;//不要用entry，和上面的entry命名冲突
                    memcpy(&insideEntry, dataBlock+i*32, 32);
                    string temp=DirName2RealName(&insideEntry);
                    if((*paths)[0]==temp){
                        
                        vector<string>::iterator k=(*paths).begin();
                        (*paths).erase(k);//删除第一个元素
                        re=dfsFindEntry(&(*paths), insideEntry);
                        return re;
                    }
            }
        }

        return re;
    }else{
        re.rightPath=true;
        re.entry=entry;
        return re;
    }
}


void dfsPrint(Entry entry,vector<string>* pathBeforeName,int type){//使用递归算法，实现深度优先遍历  引用被初始化后不可被改变？
    int attr=(unsigned char)(entry.DIR_Attr);
    string temp=DirName2RealName(&entry);
    bool bb=temp==".";
    if(attr==16 && DirName2RealName(&entry)!="." && DirName2RealName(&entry)!=".."){
        visitRelativeRoot(entry, &(*pathBeforeName), type);

        (*pathBeforeName).push_back(DirName2RealName(&entry));
        int clusNum=char2int(entry.DIR_FstClus, 0, 2);
        while(clusNum>0){
            char dataBlock[512];//硬编码
            virtualDiskHead(dataBlock, DataBase+clusNum-2, 0, 512);
            clusNum=fatTableReader(clusNum);
            for (int i = 0; i < BPB_BytsPerSec / sizeof(Entry); i++) {
                    Entry insideEntry;//不要用entry，和上面的entry命名冲突
                    memcpy(&insideEntry, dataBlock+i*32, 32);
                    dfsPrint(insideEntry,&(*pathBeforeName),type);
            }
        }
        (*pathBeforeName).pop_back();
    }
    else if(DirName2RealName(&entry)=="." || DirName2RealName(&entry)==".."){// if(DirName2RealName(&entry)=="." || DirName2RealName(&entry)=="..")，用or不是and！！！！！
        //不输出什么，在visitRelativeRoot里已经输出了 ". ..+目录下的所有文件"    
    }
}
void visitRelativeRoot(Entry entry, vector<string>* pathBeforeName, int type){//提供给printChildren调用，相当于二叉树遍历的访问根节点
    string toPrint="";
    int aa=(*pathBeforeName).size();
    for(int i=0;i<(*pathBeforeName).size();i++){
        toPrint+=string((*pathBeforeName)[i])+"/";
    }
    toPrint+=DirName2RealName(&entry);
    toPrint="/"+toPrint;
    /*
    输出文件下的目录数和文件数量
    */
    if(type==ls){
        toPrint+="/:";
        asm_print(const_cast<char*>(toPrint.c_str()));
        asm_print("\n");
        redStart();
        asm_print(".  ..  ");
        redEnd();
    }
    else{
        toPrint+="/";
        asm_print(const_cast<char*>(toPrint.c_str()));
        asm_print("  ");
        printCount(entry);
        asm_print(":");
        asm_print("\n");
        redStart();
        asm_print(".\n..\n");
        redEnd();
    }


    int clusNum=char2int(entry.DIR_FstClus, 0, 2);
    while(clusNum>0){
        char dataBlock[512];//硬编码
        virtualDiskHead(dataBlock, DataBase+clusNum-2, 0, 512);
        clusNum=fatTableReader(clusNum);
        for (int i = 0; i < BPB_BytsPerSec / sizeof(Entry); i++) {
            Entry insideEntry;//不要用entry，和上面的entry命名冲突
            memcpy(&insideEntry, dataBlock+i*32, 32);
            int attr=(unsigned char)(insideEntry.DIR_Attr);
            string printCurrentPath_FileAndDirec=DirName2RealName(&insideEntry);

            int char0=char2int(insideEntry.DIR_Name,0,1);
            if( (attr==16||attr==32) && (DirName2RealName(&insideEntry)!="." && DirName2RealName(&insideEntry)!="..") && char0!=229){
                if(type==ls){
                    if(attr==16){
                        redStart();
                        asm_print(printCurrentPath_FileAndDirec.c_str());
                        redEnd();
                        asm_print("  ");
                    }else{
                        asm_print(printCurrentPath_FileAndDirec.c_str());
                        asm_print("  ");
                    }
                    
                }
                else if(attr==16){
                    redStart();
                    asm_print(printCurrentPath_FileAndDirec.c_str());
                    redEnd();
                    asm_print("  ");
                    printCount(insideEntry);
                    asm_print("\n");
                }else if(attr==32){
                    asm_print(printCurrentPath_FileAndDirec.c_str());
                    asm_print("  ");
                    int fileSize=char2int(insideEntry.DIR_FileSize,0,4);
                    asm_print(int2String(fileSize).c_str());
                    asm_print("\n");
                }
            }
        }
    }
    asm_print("\n");
}


//ls -l时使用， 打印目录和文件数
void printCount(Entry entry){ 
    int clusNum=char2int(entry.DIR_FstClus, 0, 2);
    int directories=0;
    int files=0;
    while(clusNum>0){
        char dataBlock[512];//硬编码
        virtualDiskHead(dataBlock, DataBase+clusNum-2, 0, 512);
        clusNum=fatTableReader(clusNum);
        for (int i = 0; i < BPB_BytsPerSec / sizeof(Entry); i++) {
            Entry insideEntry;//不要用entry，和上面的entry命名冲突
            memcpy(&insideEntry, dataBlock+i*32, 32);
            int attr=(unsigned char)(insideEntry.DIR_Attr);
            int char0=char2int(insideEntry.DIR_Name,0,1);
            if( (attr==16||attr==32) && (DirName2RealName(&insideEntry)!="." && DirName2RealName(&insideEntry)!="..") && char0!=229){
                if(attr==16)
                    directories++;
                else if(attr==32)
                    files++;
            }      
        }
    }
    asm_print(int2String(directories).c_str());
    asm_print(" ");
    asm_print(int2String(files).c_str());;
}
void printRootCount(){
    int entryNum=0;
    int directories=0;
    int files=0;
    while(entryNum<BPB_RootEntCnt){
        char anEntry[32];
        virtualDiskHead(anEntry, RootDirBase, entryNum*32, 32);
        Entry loadEntry;
        memcpy(&loadEntry, anEntry, 32);//将数据结构化
        int dirAttr=(unsigned char)(loadEntry.DIR_Attr);
        int char0=char2int(loadEntry.DIR_Name,0,1);
        if((dirAttr==16 || dirAttr==32) && char0!=229){
            if(dirAttr==16)
                directories++;
            else
                files++;
        }
        entryNum++;
    }
    asm_print(int2String(directories).c_str());
    asm_print(" ");
    asm_print(int2String(files).c_str());;
}


void methodForCat(Entry entry);
void methodForCat(Entry entry){
    int clusNum=char2int(entry.DIR_FstClus, 0, 2);
    int fileSize=char2int(entry.DIR_FileSize,0,4);
    int count=0;
    while(clusNum>0){
        char dataBlock[512];//硬编码
        virtualDiskHead(dataBlock, DataBase+clusNum-2, 0, 512);
        clusNum=fatTableReader(clusNum);
        int n=0;
        while(count<fileSize && n<512){//char数组用cout输出，结尾不加\0会有问题
            asm_print(char2String(dataBlock[n]).c_str());
            n++;
            count++;
        } 
    }
}
/*
    main函数
*/
int main()
{
    initializeIMG("./a.img");
    initializeBPB();
    string command;
    vector<string> args;//?移到括号里会发生有错误,那就在使用完参数后pop出来
    while(true){
        asm_print(" > ");
        getline(cin, command);
        int userCommand=commandParser(command, &args);
        if(userCommand==INEFFECTIVE_COMMAND)
            continue;
        else if(userCommand==myexit){
            break;
        }
        else if(userCommand==ls || userCommand==lsWithOptionL){
            if(args.size()==0){
                printRootEntries(userCommand);
            }
            else if(args.size()==1){
                //以‘/’分割路径
                string userPath=args[0];
                args.pop_back();
                vector<string> paths;

                parsePath(userPath,&paths);
                EntryResult re=findEntry(&paths);
                if(!re.rightPath){
                    asm_print("没有查询到您输入的目录\n请检查输入的目录格式:不带参数表示根目录;带参数的格式为/directory1[/directory2]*");
                    asm_print("\n");
                    continue;
                }
                //找到该路径
                parsePath(userPath,&paths);//因为在findEntry(&paths)时改变了paths的值，所以重新解析   如果找到了这个路径，paths已经是空，不需要clear
                paths.pop_back();//最后一个目录会在dfsPrint里输出
                dfsPrint(re.entry,&paths,userCommand);
            }
        }
        else{
            //cat
            //以‘/’分割路径
            string userPath=args[0];
            args.pop_back();//拿出来之后清空args，防止下一个循环开始时args不是空，出现错误
            vector<string> paths;

            parsePath(userPath,&paths);
            EntryResult re=findEntry(&paths);//得到了这个文件的entry
            if(!re.rightPath){
                asm_print("当前目录下没有查询到该文件\n");
                continue;
            }
            methodForCat(re.entry);
        }
    }
    return 0;
}


//初始化方法
void initializeIMG(const char* imgPath){
        FILE *fp = fopen(imgPath, "r");
        if (fp==NULL) {
            invalidImg();
        }
        int judge = fread(IMG, IMAGE_SIZE, 1, fp);//将文件里的内容放到IMG，便于之后直接从IMG读取，而不用使用指针在文件里不断偏移
        fclose(fp);
        if(judge!=1){
            invalidImg();
        }
}
void initializeBPB(){
        BPB_BytsPerSec=char2int(IMG,11,2);
        BPB_SecPerClus=char2int(IMG,13,1);
        BPB_RsvdSecCnt=char2int(IMG,14,2);
        BPB_NumFATs=char2int(IMG,16,1);
        BPB_RootEntCnt=char2int(IMG,17,2);
        BPB_TotSec16=char2int(IMG,19,2);
        BPB_FATSz16=char2int(IMG,22,2);
        BPB_SecPerTrk=char2int(IMG,24,2);
        BPB_HiddSec=char2int(IMG,28,4);
        BPB_TotSec32=char2int(IMG,32,4);
        memcpy(BS_FileSysType,IMG+54,8);//文件类型，用字符描述，直接copy memcpy不会在结尾多加一个\0

        Fat1Base=BPB_RsvdSecCnt;
        RootDirBase=Fat1Base+BPB_FATSz16*2;
        DataBase=RootDirBase+((BPB_RootEntCnt * sizeof(Entry)) + (BPB_BytsPerSec - 1)) / BPB_BytsPerSec;
}
//一些支持的方法 util
void virtualDiskHead (char* re, int secNum, int begin, int size) {
    for (int i = 0 ; i < size; ++i) {
        re[i] = IMG[secNum * BPB_BytsPerSec + begin + i];
    }
}
int char2int (const char* theChars, int begin, int size) {//初始化512字节时，char转化为int十进制
    int num = 0;
    for (int i = begin + size - 1; i >= begin; i--) {
        num = num * 256 + (unsigned char) theChars[i];
    }
    return num;
}
void invalidImg(){
    asm_print("不是合法的 1.44M FAT12软盘！");
    exit(-1);//异常退出
}
/*
    命令解析器  判断符号串是否为合法命令，如果是，判断是哪一种命令，并解析出相应的参数（args）
    ---args：①ls是路径名、②cat是文件名
    如果用户的输入是不合法的->给出相应的较为详细的提示，让用户更正输入
    合法情况：
    1.exit
    2.ls
    3.ls带有-l选项
    4.cat
    输入的s是用getline读取的一行字符，不包括回车
*/
int commandParser(const string s, vector<string>* const args ){//vector内容可以更改，但是这两个指针指向的地方不能变
    int commandKind=INEFFECTIVE_COMMAND;
    string cmdStr="";

    if(s.size()==0){
        asm_print("不要啥都不告诉我呀，请输入一段指令哦");
        asm_print("\n");
        return commandKind;//多处返回，但是避免将下面很长的代码块放在else里
    }

    int i=0;
    while(i<s.size()){//command是const不变的，所以这里直接使用.size()，而没有将size保存到一个变量里是可行的
        if(s[i]!=' ')
            break;//命令行输入时，允许用户在正式输入一条命令之前误打了一些空格
        i++;
    }
    //***********注释代码块考虑了输入全部是空格的情况
    if(i==s.size()){
        asm_print("不要啥都不告诉我呀，请输入一段指令哦");
        asm_print("\n");
        return commandKind;//多处返回，但是避免将下面很长的代码块放在else里
    }

    //ifelse分三种情况，ls里有两种情况
    if(s[i]=='e'){
        cmdStr+=s[i];
        while(i+1<s.size() && s[i+1]!=' '){
            cmdStr+=s[i+1];
            i++;
        }
        //开头为e，则只有exit是合法的
        if(cmdStr=="exit"){//string类的比较可直接用==
            commandKind=myexit;
            //判断exit之后有没有除了空格的别的符号
            while(i+1<s.size()){
                if(s[i+1]!=' '){//***************exit之后只跟一些[空格]的情况
                    commandKind=INEFFECTIVE_COMMAND;
                    asm_print("exit后不能加任何参数！如果你想退出本程序，请输入exit并回车");
                    asm_print("\n");
                    break;
                }
                i++;
            }
        }else{
            asm_print("如果你想退出本程序，请输入exit并回车\n");
        }


    }//END IF

    else if(s[i]=='l'){
        cmdStr+=s[i];
        while(i+1<s.size() && s[i+1]!=' '){
            cmdStr+=s[i+1];
            i++;
        }
        //开头为l，则只有ls是合法的  
        if(cmdStr=="ls"){
            commandKind=ls;
            int argsCount=0;

            i++;//i指向ls后的第一个字符
            while(i<s.size()){
                if(s[i]=='-'){
                    int countL=0;
                    int countChars=0;
                    while(i + 1 < s.size() && s[i + 1] != ' ') {
                        if(s[i+1]=='l')
                            countL++;
                        countChars++;
                        i++;
                    }
                    //根据while内循环的结果，判断-option是不是合法的  注意：-后面至少一个参数才是合法的
                    if(countL==countChars && countChars!=0)
                        commandKind=lsWithOptionL;
                    else{
                        commandKind=INEFFECTIVE_COMMAND;
                        asm_print("ls可带的选项只有-l\n");
                        asm_print("请检查：'-'符号后是否缺漏参数，是否跟随'l'以外的其他非法参数！\n");
                        asm_print("推荐的输入格式:\nls -l /Directory[tip:有且仅有一条目录作为参数]\n");//如果有多条目录错误，这里会以tip的形式给出
                        break;//或：return commandKind
                    }
                }
                else if(s[i]!=' '){
                    string userArg = "";
                    userArg += s[i];
                    while (i + 1 < s.size() && s[i + 1] != ' ') {
                        userArg += s[i + 1];
                        i++;
                    }
                    argsCount++;
                    if(argsCount>1){
                        commandKind=INEFFECTIVE_COMMAND;
                        asm_print("ls后只能只能加一条路径！\n");
                        asm_print("推荐的输入格式:\nls [-l] /Directory[只有一条路径哦]\n");//如果有-l的错误，这里会以tip的形式给出-----用户命令首先检测到目录条数错误，要求更改；由于ls后只有两个选项，给出规范格式，用户自检另外一个参数即可；用户未检测出，在下一次错误时也会要求更改
                        break;
                    }
                    args->push_back(userArg);//参数全部加进来后再判断参数有没有超过一个
                }
                i++;
            }//END while  ls内的while循环-用作读取ls指令


        }else//l开头，但是不是ls指令
            asm_print("如果你想显示某个目录的结构，请按如下格式输入命令:ls /Directory\n");

    }//EnD IF

    else if(s[i]=='c'){
        cmdStr+=s[i];
        while(i+1<s.size() && s[i+1]!=' '){
            cmdStr+=s[i+1];
            i++;
        }
        //开头为c，则只有cat是合法的
        if(cmdStr=="cat"){
            commandKind=cat;
            int argsCount=0;
            i++;//i指向cat后的第一个字符
            while(i<s.size()){
                if(s[i]!=' '){
                    string userArg="";
                    userArg += s[i];
                    while (i + 1 < s.size() && s[i + 1] != ' ') {
                        userArg += s[i + 1];
                        i++;
                    }
                    argsCount++;
                    if(argsCount>1){
                        commandKind=INEFFECTIVE_COMMAND;
                        asm_print("cat后只能加一个文件名！\n");
                        break;
                    }
                    args->push_back(userArg);
                }
                i++;
            }

        }else
            asm_print("如果你想显示某个文件的内容，请按如下格式输入命令:cat fileName.fileExt\n");

    }
    else{
        asm_print("这个命令我不认识呀，请从ls、cat、exit里选择一条指令哦\n");
    }

    return commandKind;
}
//根据当前簇号，到fat表对应位置查找下一个簇号
int fatTableReader(int clusNum){
    char fatData[2];
    virtualDiskHead(fatData,1,clusNum*3/2,2);//fat表从第一个扇区开始
    //clusNum为偶数去掉第2个字节的高四位，也就是保留右边低4位
    //clusNum为奇数去掉第1个字节的低四位，也就是保留左边高4位
    //没有用unsigned char数据类型，移动比较烦，先转为unsigned，再扩展为short再移动...
    int nextClusNum=-1;
    if(clusNum%2==0){
        nextClusNum=((unsigned char) fatData[1]) % 16 * 256 + ((unsigned char) fatData[0]);//%16取低4位，*256左移8位
    }else{
        nextClusNum=((unsigned char) fatData[1]) * 16 + ((unsigned char) fatData[0]) / 16;// /16右移4位取高4位，*16左移4位
    }
    if (nextClusNum >= 0xff7 || nextClusNum <= 1) {//簇号从2开始 0xff7坏簇 大于则表示已经到结尾
            nextClusNum = -1;
    }
    return nextClusNum;
}
//解析目录和文件名
string DirName2RealName(Entry *entry){
    char* dirName=entry->DIR_Name;//可以直接将char[]给char* char[]到char[]要用memcpy
    string re="";
    if((unsigned char)(entry->DIR_Attr)==16){//目录类型
        string directoryName(dirName,8);//这个函数存在隐式的char*到string的转化
        while(directoryName.size()>0 && directoryName[directoryName.size()-1]==' ')
            directoryName=directoryName.substr(0,directoryName.size()-1);//去除空格
        re=directoryName;
    }else if((unsigned char)(entry->DIR_Attr)==32){//文件类型
        string fileName(dirName,8);
        string extName(dirName,8,3);
        while(fileName.size()>0 && fileName[fileName.size()-1]==' ')
            fileName=fileName.substr(0,fileName.size()-1);
        while(extName.size()>0 && extName[extName.size()-1]==' ')
            extName=extName.substr(0,extName.size()-1);
        re=fileName+"."+extName;
    }
    return re;
}
//  /NJU/CS -> {"NJU","CS"}
void parsePath(string userPath,vector<string>* paths){
    for(int i=0;i<userPath.size();i++){
        if(userPath[i]=='/')
            userPath[i]=' ';
    }
    istringstream out(userPath);
    string str;
    while(out>>str){
        (*paths).push_back(str);
    }
}
// void parsePath(string userPath,vector<string>* paths){
//     char* tokenPtr=strtok(const_cast<char*>(userPath.c_str()),"/");
//     while(tokenPtr!=NULL){
//         string tmp(tokenPtr);
//         (*paths).push_back(tmp);
//         tokenPtr=strtok(NULL,"/");
//     }
// }