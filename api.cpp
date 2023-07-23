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
char res_tmp[6];
char ans_tmp[6];
char keys_tmp[6];
std::string ans;
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
    int cdl_is_valid(char *str)
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
    char *cdl_take_guess(char *str)
    {
        std::string guess = "AAAAA";
        guess[0] = str[0];
        guess[1] = str[1];
        guess[2] = str[2];
        guess[3] = str[3];
        guess[4] = str[4];
        guess = cdl_round->take_a_guess(guess);
        res_tmp[0] = guess[0];
        res_tmp[1] = guess[1];
        res_tmp[2] = guess[2];
        res_tmp[3] = guess[3];
        res_tmp[4] = guess[4];
        return res_tmp;
    }
    EMSCRIPTEN_KEEPALIVE
    char *cdl_show_answer()
    {
        std::string guess = cdl_round->answer;
        ans_tmp[0] = guess[0];
        ans_tmp[1] = guess[1];
        ans_tmp[2] = guess[2];
        ans_tmp[3] = guess[3];
        ans_tmp[4] = guess[4];
        return ans_tmp;
    }
    EMSCRIPTEN_KEEPALIVE
    char *cdl_get_keyboard_status(char *keys)
    {
        for (int i = 0; i < 5; i++)
        {
            switch (cdl_round->keyboard->get_status(keys[i]))
            {
            case 0:
                keys_tmp[i] = 'G';
                break;
            case 1:
                keys_tmp[i] = 'Y';
                break;
            case 2:
                keys_tmp[i] = 'R';
                break;
            default:
                keys_tmp[i] = 'X';
                break;
            };
        }
        return keys_tmp;
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
        int i = distribution(randomEngine) % FIN_NUM;
        ans = "apple";
        ans[0] = final[i][0];
        ans[1] = final[i][1];
        ans[2] = final[i][2];
        ans[3] = final[i][3];
        ans[4] = final[i][4];
        cdl_round->answer = ans;
    };

#ifdef __cplusplus
}
#endif