#include "BOBHash32.h"

uint32_t po(unsigned x){
    if(x==0) return 1;
    return 2*po(x-1);
}
template <const int tuple_len>
class Sketch{
public:
    unsigned TYPE_BIT, HASH_NUM, BUCK_NUM, TRACE_LEN, TYPE_NUM;
    BOBHash32 typehash;
    BOBHash32 * skehash;
    uint16_t * bucket;
    Sketch(const unsigned int typebit, const unsigned int hashnum, const unsigned int bucknum, const unsigned int tracelen){
    /*
    typebit is k from 2^k=typenum, hashnum is number of hash functions, bucknum is number of buckets
    */
        TYPE_BIT = typebit, HASH_NUM = hashnum, BUCK_NUM = bucknum, TRACE_LEN = tracelen;
        TYPE_NUM = po(TYPE_BIT);

        bucket = new uint16_t[BUCK_NUM];
        memset(bucket, 0, sizeof(uint16_t)*BUCK_NUM);
    
        skehash = new BOBHash32[HASH_NUM];
        unsigned seed = rand() % MAX_PRIME32;
        for(int i = 0; i < HASH_NUM; i++){
            skehash[i].initialize((seed+i)%MAX_PRIME32);
        }
        typehash.initialize(3);
    }
    unsigned get_type(uint8_t key[]){
        return typehash.run((char*)key, tuple_len) % TYPE_NUM;
    }
    void insert(uint8_t key[]){
        unsigned th = TYPE_BIT / 2;
        unsigned cntsame = 0;
        unsigned cntzero = 0;
        unsigned tmp = 0;
        uint32_t tmptype = get_type(key) % TYPE_NUM;
        for(int i = 0; i < HASH_NUM; i++){
            uint32_t pos = skehash[i].run((char*)key, tuple_len) % BUCK_NUM;
            uint32_t postype = (pos >> (16-TYPE_BIT));

            if (tmptype == postype && bucket[pos] != 0) cntsame += 1;
            if (bucket[pos] == 0) cntzero += 1, tmp = pos;
        }

        //complex insert rules, with bugs on high possibility
        if(cntsame < th && cntsame != 0){
            if(cntzero != 0){
                bucket[tmp] = (uint16_t) tmptype;
                bucket[tmp] <<= (16-TYPE_BIT);
                bucket[tmp] += 1;
            }
            else{
                for(int i = 0; i < HASH_NUM; i++){
                    uint32_t pos = skehash[i].run((char*)key, tuple_len) % BUCK_NUM;
                    uint32_t postype = (bucket[pos] >> (16-TYPE_BIT));
                    if (tmptype == postype){
                        bucket[pos] += 1;
                    }
                }

            }
        }
        else if (cntsame == 0){
            for(int i = 0; i < HASH_NUM; i++){
                uint32_t pos = skehash[i].run((char*)key, tuple_len) % BUCK_NUM;
                bucket[pos] += 1;
            }
        }
        else{
            for(int i = 0; i < HASH_NUM; i++){
                uint32_t pos = skehash[i].run((char*)key, tuple_len) % BUCK_NUM;
                uint32_t postype = (bucket[pos] >> (16-TYPE_BIT));
                if (tmptype == postype){
                    bucket[pos] += 1;
                }
            }
        }
    } 

    uint16_t query(uint8_t key[]){
        uint16_t classmin = 0x3fff;
        uint16_t allmin = 0x3fff;
        uint32_t keytype = get_type(key) % TYPE_NUM;
        for(int i = 0; i < HASH_NUM; i++){
            uint32_t pos = skehash[i].run((char*)key, tuple_len) % BUCK_NUM;
            uint32_t postype = (bucket[pos] >> (16-TYPE_BIT));
            if(keytype == postype){
                if((0x3fff & bucket[pos]) < classmin ) classmin = (0x3fff & bucket[pos]);
            }
            //cout << "..."<< bucket[pos] << endl;
            if((0x3fff & bucket[pos]) < allmin) {allmin = (0x3fff & bucket[pos]);
            }
        }

        if(classmin == 0x3fff) return allmin;
        else return classmin;
    }
    ~Sketch(){
        if(skehash) delete[] skehash;
        if(bucket) delete[] bucket;
    }
};

template <const int tuple_len>
class CMSketch{
public:
    unsigned HASH_NUM, BUCK_NUM, TRACE_LEN;
    BOBHash32 * skehash;
    uint16_t * bucket;
    CMSketch(const int hashnum, const int bucknum, const int tracelen):HASH_NUM(hashnum), BUCK_NUM(bucknum), TRACE_LEN(tracelen){
        //initialize hash functions and buckets
        //if(bucket){
         //   delete[] bucket;
        //    bucket = NULL;
        //}
        //cout << "lalala" << endl;
        bucket = new uint16_t[BUCK_NUM];
        memset(bucket, 0, sizeof(uint16_t)*BUCK_NUM);
        //cout << sizeof(bucket) << endl;
        skehash = new BOBHash32[HASH_NUM];
        unsigned seed = rand() % MAX_PRIME32;
        for(int i = 0; i < HASH_NUM; i++){
            skehash[i].initialize((seed+i)%MAX_PRIME32);
        }
    }

    void insert(uint8_t * key){
        for(int i = 0; i < HASH_NUM; i++){
            uint32_t pos = skehash[i].run((char*)key, tuple_len) % BUCK_NUM;
            bucket[pos] += 1;
            //cout << pos << ',' << bucket[pos] << ',';
            //system("pause");
        }
        //for(int i = 0; i < BUCK_NUM; i++)cout << bucket[i] << ',';
        //cout << endl;
    }
    uint16_t query(uint8_t key[]){
        uint16_t Min = 0xffff;
        for(int i = 0;i < HASH_NUM; i++){
            uint32_t pos = skehash[i].run((char*)key, tuple_len) % BUCK_NUM;
            //cout << "cmquery" << pos << endl;
            if(bucket[pos] < Min) Min = bucket[pos];
        }
        return Min;
    }
    ~CMSketch(){
        if(skehash) delete[] skehash;
        if(bucket) delete[] bucket;
    }
};