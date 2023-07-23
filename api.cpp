#include <random>

#include <emscripten/emscripten.h>
#include "wordle.hpp"

std::default_random_engine randomEngine;
std::uniform_int_distribution<int> distribution;

void initRandom(int seed)
{
    randomEngine = std::default_random_engine(seed);
}

cordle::Player *cdl_player = nullptr;
cordle::Round *cdl_round = nullptr;

std::string cur_input_status = "";

#ifdef __cplusplus
extern "C"
{
#endif
    EMSCRIPTEN_KEEPALIVE
    int cdl_is_success()
    {
        return cdl_round->is_success;
    }
    EMSCRIPTEN_KEEPALIVE
    void cdl_set_mode(int is_hard)
    {
        cdl_round->set_hardness(is_hard);
    }
    EMSCRIPTEN_KEEPALIVE
    int cdl_is_vaild(char *str)
    {
        std::string guess = "AAAAA";
        guess[0] = str[0];
        guess[1] = str[1];
        guess[2] = str[2];
        guess[3] = str[3];
        guess[4] = str[4];
        return cdl_round->is_valid(guess);
    }
    EMSCRIPTEN_KEEPALIVE
    void cdl_take_guess(char *str)
    {
        std::string guess = "AAAAA";
        guess[0] = str[0];
        guess[1] = str[1];
        guess[2] = str[2];
        guess[3] = str[3];
        guess[4] = str[4];
        cdl_round->take_a_guess(guess);
    }
    EMSCRIPTEN_KEEPALIVE
    int cdl_get_input_status(int index)
    {
        switch (cur_input_status[index])
        {
        case 'G':
            return 0;
            break;
        case 'Y':
            return 1;
            break;
        case 'R':
            return 2;
            break;
        default:
            return 3;
            break;
        }
    }
    EMSCRIPTEN_KEEPALIVE
    int cdl_get_keyboard_status(int key)
    {
        return cdl_round->keyboard->get_status(key);
    }
    EMSCRIPTEN_KEEPALIVE
    void cdl_init(int seed)
    {
        if (cdl_player != nullptr)
            delete cdl_player;
        if (cdl_round != nullptr)
            delete cdl_round;
        cdl_player = new cordle::Player();
        cdl_round = new cordle::Round();
        initRandom(seed);
        std::string ans = final[distribution(randomEngine) % ACC_NUM];
        cdl_round->set_answer(ans);
    };

#ifdef __cplusplus
}
#endif