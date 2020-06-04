#include <iostream>
#include <vector>
#include <map>

using namespace std;

static vector<int> possibilities;
static vector<int> all_codes;

vector<int> generatePossibilities() {
    vector<int> gen;
    for (int i = 1; i < 7; i++) {
        for (int j = 1; j < 7; j++) {
            for (int k = 1; k < 7; k++) {
                for (int l = 1; l < 7; l++) {
                    int num = 1000 * i + 100 * j + 10 * k + l;
                    gen.push_back(num);
                }              
            }
        }
    }
    return gen;
}

void remove_code(vector<int> &set, int currentCode) {
    int index;
    for (auto it = set.begin(); it != set.end(); it++) {
        index = distance(set.begin(), it);

        if (set[index] == currentCode) {
            set.erase(set.begin() + index);
            break;
        }
    }
}

string check_code(vector<int> cur_guess, vector<int> variable_guess) {
    string gen_hint = "";

    //reds
    for (int j = 0; j < 4; j++) {
        //cout << cur_guess[j] << variable_guess[j] << endl;
        if (cur_guess[j] == variable_guess[j]) {
            gen_hint.append("r");
            cur_guess[j] *= -1;
            variable_guess[j] *= -1;
        }
    }

    //whites
    for (int j = 0; j < 4; j++) {
        if (cur_guess[j] > 0) {
            vector<int>::iterator itr = find(variable_guess.begin(), variable_guess.end(), cur_guess[j]);
            if (itr != variable_guess.end()) {
                int index = distance(variable_guess.begin(), itr);
                gen_hint.append("w");
                variable_guess[index] *= -1;
            }
        }
    }

    if (gen_hint == "") {
        gen_hint = "0";
    }

    //cout << gen_hint << endl;

    return gen_hint;
}

vector<int> trim_set(vector<int> possibilities, int guess, string hint) {
    vector<int> edited = possibilities;
    //for every element in edited, compare with the guess

    string gen_hint;

    int counter = 0;
    int c2 = 0;

    int index;
    vector<int>::iterator it = edited.begin();
    vector<int> cur_guess;

    cout << guess << endl;

    while (it != edited.end()) {
        index = distance(edited.begin(), it);

        cur_guess.clear();

        int temp1 = guess;
        for (int i = 0; i < 4; i++) {
            cur_guess.insert(cur_guess.begin(), temp1 % 10);
            temp1 /= 10;
        }

        cout << cur_guess[0] << cur_guess[1] << cur_guess[2] << cur_guess[3] << endl;

        //make vector
        int temp = edited[index];
        vector<int> variable_guess;
        for (int k = 0; k < 4; k++) {
            variable_guess.insert(variable_guess.begin(), temp % 10);
            temp /= 10;
        }

        cout << variable_guess[0] << variable_guess[1] << variable_guess[2] << variable_guess[3] << endl;

        gen_hint = check_code(cur_guess, variable_guess);

        vector<int> v1{6,3,3,1};
        if (variable_guess == v1) {
            cout << "here!" << endl;
            cout << hint << " | " << gen_hint << endl;
        }

        if (hint != gen_hint) {
            counter++;
            it = edited.erase(edited.begin() + index);
            //cout << "removed!" << endl;
        } else {
            it++;
        }

        c2++;
        //cout << "pass: " << c2 << endl;
    }

    //cout << "num found: " << counter << endl;
    cout << "remaining possible solns: " << edited.size() << endl;
    //cout << "first ele" << *edited.begin() << endl;
    return edited;
}

vector<int> min_max() {
    vector<int> optimal_guesses;
    map<string, int> hit_count;
    map<int, int> guess_scores;

    for (int i = 0; i < all_codes.size(); ++i) {
        vector<int> cur_guess;
        int temp1 = all_codes[i];
        for (int i = 0; i < 4; i++) {
            cur_guess.insert(cur_guess.begin(), temp1 % 10);
            temp1 /= 10;
        }

        for (int j = 0; j < possibilities.size(); ++j) {

            //make vector
            int temp = possibilities[j];
            vector<int> variable_guess;
            for (int k = 0; k < 4; k++) {
                variable_guess.insert(variable_guess.begin(), temp % 10);
                temp /= 10;
            }

            string hint_pegs = check_code(cur_guess, variable_guess);
            if (hit_count.count(hint_pegs) > 0) {
                hit_count.at(hint_pegs)++;
            } else {
                hit_count.emplace(hint_pegs, 1);
            }
        }

        //get minimum elims
        int max = 0;

        for (auto entry : hit_count) {
            if (entry.second > max) {
                max = entry.second;
            }
        }

        guess_scores.emplace(all_codes[i], max);
        hit_count.clear();
    }

    int min = INT_MAX;

    for (auto entry : guess_scores) {
        if (entry.second < min) {
            min = entry.second;
        }
    }

    for (auto entry : guess_scores) {
        if (entry.second == min) {
            optimal_guesses.push_back(entry.first);
        }
    }

    return optimal_guesses;
}

int generateGuess(vector<int> possibilities, vector<int> all_codes) {
    vector<int> optimal_guesses = min_max();
    int guess;
    for (int i = 0; i < optimal_guesses.size(); ++i) {
        if (find(possibilities.begin(), possibilities.end(), optimal_guesses[i]) != possibilities.end()) {
            return optimal_guesses[i];
        }
    }
    for (int i = 0; i < optimal_guesses.size(); ++i) {
        if (find(all_codes.begin(), all_codes.end(), optimal_guesses[i]) != all_codes.end()) {
            return optimal_guesses[i];
        }
    }
    return guess;
}

int main() {
    int turns = 1;

    possibilities = generatePossibilities();
    all_codes = generatePossibilities();
    cout << "i am guess number man" << endl;
    
    int guess = 1122;
    bool won = false;

    cout << "Turn: " << turns << endl << "Guess: " << guess << endl;
    
    while (!won) {
        string hint;
        bool valid = true;
        do {
            valid = true;
            cout << "enter hint pegs" << endl;
            cin >> hint;
            if (hint != "0") {
                for (int i = 0; i < hint.length(); i++) {
                    if (hint[i] != 'r' && hint[i] != 'w') {
                        valid = false;
                        break;
                    }
                }
            }
        } while (!valid);
        if (hint == "rrrr") {
            won = true;
            break;
        }

        remove_code(possibilities, guess);
        remove_code(all_codes, guess);

        possibilities = trim_set(possibilities, guess, hint);

        for (auto i = possibilities.begin(); i != possibilities.end(); i++) {
            cout << *i << endl;
        }

        guess = generateGuess(possibilities, all_codes);
        turns++;
        cout << "Turn: " << turns << endl << "Guess: " << guess << endl;

    }
    cout << "ggs, turns taken: " << turns << endl;
    //bool contained = possibilities.find(element) != possibilities.end();
    return 0;
}

