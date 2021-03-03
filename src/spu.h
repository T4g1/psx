#ifndef SPU_H
#define SPU_H

#define SPU_START               0x1F801C00
#define SPU_SIZE                640


/**
 * @brief      Sound Processing Unit
 */
class SPU {

public:
    ~SPU();

    bool init();
    void reset();
};

#endif /* SPU_H */
