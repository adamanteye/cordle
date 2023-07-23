#include <set>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>

#include <emscripten/emscripten.h>

#define LETTERS_NUM 26
#define WORD_LENGTH 5

namespace cordle
{
    int conunt_letter(const std::string word, char letter)
    {
        int cnt = 0;
        int pos = word.find_first_of(letter);
        while (pos != word.npos)
        {
            cnt++;
            pos = word.find_first_of(letter, pos + 1);
        }
        return cnt;
    }
    //  Convert a lowercase letter to uppercase
    std::string upper(const std::string word)
    {
        std::string res = "";
        for (int index = 0; index < word.length(); index++)
            res += word[index] - 'a' + 'A';
        return res;
    }
    //! relative priorities: G>Y>R>X
    enum Status
    {
        hit,    // G, Green for exactly correct
        close,  // Y, Yellow for being at wrong position
        missed, // R, Red for more than they are
        unknown // X, Whether correct or not is unknown
    };
    class Keyboard
    {
    public:
        // Initialize all keys by unknown
        Keyboard();
        // Get status of a letter
        Status get_status(char letter);
        //! It is possible that status is not changed, due to relative priorities
        //* Return true if status is changed after being called
        bool update_status(char letter, Status status);

    private:
        Status keys[LETTERS_NUM];
        // Set a letter to a given status
        void set_status(char letter, Status status);
    };

    class WordSet
    {
    public:
        // Initialize final_set and acceptable_set from specific files
        WordSet(const std::string final_file_path, const std::string acceptable_file_path);
        // Return true if a word is in FINAL.txt, i.e. final set
        bool in_final_set(std::string word);
        // Return true if a word is in ACC.txt, i.e. acceptable set
        bool in_acceptable_set(std::string word);

    private:
        std::set<std::string> final_set;
        std::set<std::string> acceptable_set;
    };
    class Player
    {
        friend class OJTest;

    private:
        std::vector<int> attempts_per_round;
        std::vector<bool> success_per_round;

    public:
        //  Return average attempts per round
        //! Attempts of failure rounds will be counted as 0
        double calc_avg_attempts();
        // Return how many rounds end by success
        int calc_success_cnt();
        // Return how many rounds end by failure
        int calc_failure_cnt();
        //! Append to attempts_per_round and success_per_round
        void update_info(int attempts, bool is_success);
    };

    class Round
    {
        friend class OJTest;

    private:
        Keyboard *keyboard;
        WordSet *word_set;
        bool hit_restrict[5];
        std::set<char> use_resctrit;
        bool is_success;
        bool is_hard;
        //! Is counted until first successful trial
        int success_cnt;
        // Player's guess stored in a round
        std::vector<std::string> guesses;
        // Player's guess result stored in a round
        std::vector<std::string> results;
        // The specified answer
        std::string answer;
        // Set the answer to a given string
        void set_answer(const std::string ans);
        // Called when a round ended and store player statistics
        void evaluate(cordle::Player *player);

    public:
        Round(const std::string final_file_path, const std::string acceptable_file_path, const bool if_hard);
        ~Round();
        //! Will be functional in hard mode
        //* Return true if a guess statisfies restriction
        bool is_valid(std::string guess);
        //! It will insert string of G, Y, R and X to results and possibly make modifications to keys' status of keyboard
        std::string take_a_guess(std::string guess);
    };
    //* This class is designed for passing the OJ, and will not be used elsewhere
    class OJTest
    {
    private:
        //! Read from stdin
        //! Value range: [1,4], representing in which task
        int get_cur_no();

        //  Print In desired format
        void oj_print(cordle::Round *game);

        const std::string final_file_path = "FINAL.txt";

        const std::string acceptable_file_path = "ACC.txt";

    public:
        //! Must be the first function called to conduct I/O operation
        //* Finish all tasks in single call
        void test();
    };
}

cordle::WordSet::WordSet(const std::string final_file_path, const std::string acceptable_file_path)
{
    std::ifstream FINAL_FILE;
    FINAL_FILE.open(final_file_path, std::ifstream::in);
    while (!FINAL_FILE.eof())
    {
        std::string str;
        FINAL_FILE >> str;
        final_set.insert(str);
    }
    std::ifstream ACCEPTABLE_FILE;
    ACCEPTABLE_FILE.open(acceptable_file_path, std::ifstream::in);
    while (!ACCEPTABLE_FILE.eof())
    {
        std::string str;
        ACCEPTABLE_FILE >> str;
        acceptable_set.insert(str);
    }
}
bool cordle::WordSet::in_acceptable_set(std::string word)
{
    return acceptable_set.find(word) != acceptable_set.end();
}
bool cordle::WordSet::in_final_set(std::string word)
{
    return final_set.find(word) != final_set.end();
}
cordle::Status cordle::Keyboard::get_status(char letter)
{
    return Keyboard::keys[letter - 'a'];
}
void cordle::Keyboard::set_status(char letter, Status status)
{
    Keyboard::keys[letter - 'a'] = status;
}
cordle::Keyboard::Keyboard()
{
    for (int i = 0; i < LETTERS_NUM; i++)
        keys[i] = cordle::Status::unknown;
}
bool cordle::Keyboard::update_status(char letter, cordle::Status status)
{
    if (status < keys[letter - 'a'])
    {
        set_status(letter, status);
        return true;
    }
    else
        return false;
}
void cordle::OJTest::oj_print(cordle::Round *game)
{
    std::cout << game->results.back() << ' ';
    for (int index = 0; index < LETTERS_NUM; index++)
        switch (game->keyboard->get_status(index + 'a'))
        {
        case hit:
            std::cout << 'G';
            break;
        case close:
            std::cout << 'Y';
            break;
        case missed:
            std::cout << 'R';
            break;
        case unknown:
            std::cout << 'X';
            break;
        default:
            break;
        }
    std::cout << std::endl;
}
int cordle::OJTest::get_cur_no()
{
    std::string read_tmp;
    std::getline(std::cin, read_tmp);
    int cur_no = read_tmp[0] - '1' + 1;
    return cur_no;
}
void cordle::OJTest::test()
{
    std::string ans, guess;
    int flag = get_cur_no();
    bool is_hard = flag >= 3;

    cordle::Player player = Player();
    do
    {
        Round single_round = Round(final_file_path, acceptable_file_path, is_hard); //! is_hard controls whether in restricted mode or not, i.e. hard mode
        std::getline(std::cin, ans);
        while (!single_round.word_set->in_final_set(ans))
        {
            std::cout << "INVALID" << std::endl;
            std::getline(std::cin, ans);
        }
        single_round.set_answer(ans);
        std::getline(std::cin, guess);
        while (single_round.guesses.size() < 6 && guess != "")
        {
            if (single_round.word_set->in_acceptable_set(guess) && single_round.is_valid(guess))
            {
                single_round.take_a_guess(guess);
                oj_print(&single_round);
            }
            else if (guess == "Y" || guess == "N" || single_round.guesses.size() == 6 || flag == 1 && single_round.is_success)
                break;
            else
                std::cout << "INVALID" << std::endl;
            std::getline(std::cin, guess);
        }
        if (single_round.is_success)
            std::cout << "CORRECT " << single_round.success_cnt << std::endl;
        else
            std::cout << "FAILED " << cordle::upper(single_round.answer) << std::endl;
        if (flag == 4)
        {
            single_round.evaluate(&player);
            std::cout << player.calc_success_cnt() << ' '
                      << player.calc_failure_cnt() << ' ';
            if (player.calc_avg_attempts() == 0.0)
                std::cout << 0 << std::endl;
            else
                std::cout << std::fixed << std::setprecision(2) << player.calc_avg_attempts() << std::endl;
        }
    } while (guess != "N" && flag != 1);
}
double cordle::Player::calc_avg_attempts()
{
    if (calc_success_cnt() == 0)
        return 0.0;
    else
    {
        double avg_attempts = 0.0;
        for (int i = 0; i < attempts_per_round.size(); i++)
            avg_attempts += attempts_per_round[i] * success_per_round[i];

        avg_attempts = avg_attempts / calc_success_cnt();
        return avg_attempts;
    }
}
int cordle::Player::calc_success_cnt()
{
    int cnt = 0;
    for (int i = 0; i < success_per_round.size(); i++)
        cnt += success_per_round[i];
    return cnt;
}
int cordle::Player::calc_failure_cnt()
{
    return success_per_round.size() - calc_success_cnt();
}
void cordle::Player::update_info(int attempts, bool is_success)
{
    attempts_per_round.push_back(attempts);
    success_per_round.push_back(is_success);
}
cordle::Round::Round(const std::string final_file_path, const std::string acceptable_file_path, const bool if_hard)
{
    keyboard = new Keyboard();
    word_set = new WordSet(final_file_path, acceptable_file_path);
    is_success = false;
    success_cnt = 0;
    is_hard = if_hard;
    for (int index = 0; index < WORD_LENGTH; index++)
        hit_restrict[index] = 0;
}
cordle::Round::~Round()
{
    delete keyboard;
    delete word_set;
}
void cordle::Round::set_answer(const std::string ans)
{
    answer = ans;
}
std::string cordle::Round::take_a_guess(std::string guess)
{
    std::string result = "XXXXX";
    int guess_cnt[LETTERS_NUM]{0};
    int ans_cnt[LETTERS_NUM]{0};
    for (int index = 0; index < WORD_LENGTH; index++)
        ans_cnt[guess[index] - 'a'] = cordle::conunt_letter(answer, guess[index]);
    for (int index = 0; index < WORD_LENGTH; index++)
        guess_cnt[guess[index] - 'a'] = cordle::conunt_letter(guess, guess[index]);
    for (int index = 0; index < guess.length(); index++)
    {
        if (guess[index] == answer[index])
        {
            keyboard->update_status(guess[index], cordle::Status::hit);
            result[index] = 'G';
            hit_restrict[index] = true;
            ans_cnt[guess[index] - 'a']--;
            guess_cnt[guess[index] - 'a']--;
        }
        else
            ;
    }
    //* Split the whole process into 2 to cover the following case:
    //* ans: abcda, guess: abaca
    for (int index = 0; index < guess.length(); index++)
    {
        if (guess[index] == answer[index])
            ;
        else if (ans_cnt[guess[index] - 'a'] == 0)
        {
            keyboard->update_status(guess[index], cordle::Status::missed);
            result[index] = 'R';
        }
        else
        {
            keyboard->update_status(guess[index], cordle::Status::close);
            result[index] = 'Y';
            ans_cnt[guess[index] - 'a']--;
            guess_cnt[guess[index] - 'a']--;
            use_resctrit.insert(guess[index]);
        }
    }
    results.push_back(result);
    if (result == "GGGGG")
    {
        is_success = true;
        success_cnt = success_cnt == 0 ? results.size() : success_cnt;
    }
    return result;
}
void cordle::Round::evaluate(cordle::Player *player)
{
    if (is_success)
        player->update_info(success_cnt, true);
    else
        player->update_info(results.size(), false);
}
bool cordle::Round::is_valid(std::string guess)
{
    if (!is_hard)
        return true;
    if (results.size() == 0)
        return true;
    for (std::set<char>::iterator it = use_resctrit.begin(); it != use_resctrit.end(); it++)
        if (guess.find(*it) == guess.npos)
            return false;
    int valid = 0;
    for (int index = 0; index < WORD_LENGTH; index++)
        if (hit_restrict[index] && guess[index] != answer[index])
            return false;
    return true;
}

int main()
{
    return 0;
}