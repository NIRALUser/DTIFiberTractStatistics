#include "csvparser.h"

csvparser::csvparser(QString filepath)
{
    file = new QFile(filepath);
    h1 = -1;
    h2 = -1;
    columns = 0;
    if(!file->open(QFile::ReadOnly)){
        throw ReadError();
    }
}

csvparser::~csvparser()
{
    delete file;
}

// read the first line and parse it as header
void csvparser::read(string header1, string header2)
{
    if(!file->isOpen())
        return;
    char buf[10000];
    if(next_line(buf,sizeof(buf))){
        char* col = strtok(buf,",");
        columns = 0;
        while(col!=NULL){
            if(header1.compare(col) == 0)
                h1 = columns;
            if(header2.compare(col) == 0)
                h2 = columns;
            columns++;
            col = strtok(NULL,",");
        }
        if(h1<0 || h2< 0){
            throw MatchError();
        }

    }else{
        throw ReadError();
    }


}

bool csvparser::read_row(char *a, char *b)
{
    if(!file->isOpen() || h1 < 0 || h2 < 0 || columns == 0)
        return false;
    char buf[10000];
    if(next_line(buf,sizeof(buf))){
        size_t count = 0;
        char* col = strtok(buf,",");
        while(col != NULL){
            const char* temp = col;
            if((int)count == h1)
                strcpy(a,temp);
            if((int)count == h2)
                strcpy(b,temp);
            count ++;
            col = strtok(NULL,",");
        }

        if(count != columns){
            throw InvalidData();
        }
        return true;
    }
    else
        return false;
}

bool csvparser::next_line(char buf[],qint64 size)
{
    if(!file->isOpen())
        return false;
    qint64 l = file->readLine(buf,size);
    if(l != -1){
        // remove newline character from array
        std::remove(buf,buf+strlen(buf)+2,'\n');
        return true;
    }
    return false;
}


void csvparser::close()
{
    if(!file->isOpen())
        return;
    file->close();
}




