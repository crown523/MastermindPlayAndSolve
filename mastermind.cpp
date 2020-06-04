/*
* Mastermind Solver
* Author: Kyle Liang
* Version: 1.0.1
*/

#include <iostream>
#include <vector>
#include <map>

using namespace std;

static const int NUM_DIGITS = 4;
static const int NUM_COLORS = 6;
static int comparisons = 0;
static vector<vector<int>> possible_solns;
static vector<vector<int>> all_codes;

void create_sets() {
    //generate all 1296 codes
    vector<int> num;
    for (int i = 1; i <= NUM_COLORS; i++) {
        for (int j = 1; j <= NUM_COLORS; j++) {
            for (int k = 1; k <= NUM_COLORS; k++) {
                for (int l = 1; l <= NUM_COLORS; l++) {
                    num.push_back(i);
                    num.push_back(j);
                    num.push_back(k);
                    num.push_back(l);
                    possible_solns.push_back(num);
                    all_codes.push_back(num);
                    num.clear();
                }              
            }
        }
    }
}

void remove_code(vector<vector<int>> &set, vector<int> currentCode) {
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

    //check for reds
    for (int i = 0; i < NUM_DIGITS; i++) {
        if (cur_guess[i] == variable_guess[i]) {
            gen_hint.append("r");
            cur_guess[i] *= -1;
            variable_guess[i] *= -1;
        }
    }

    //check for whites
    for (int i = 0; i < NUM_DIGITS; i++) {
        if (cur_guess[i] > 0) {
            vector<int>::iterator itr = find(variable_guess.begin(), variable_guess.end(), cur_guess[i]);
            if (itr != variable_guess.end()) {
                int index = distance(variable_guess.begin(), itr);
                gen_hint.append("w");
                variable_guess[index] *= -1;
            }
        }
    }

    //no hint pegs
    if (gen_hint == "") {
        gen_hint = "0";
    }

    comparisons++;
    return gen_hint;
}

void trim_set(vector<int> cur_guess, string hint) {
    //for every element in edited, compare with the guess
    //if it doesn't generate the same code, delete it

    int index;
    auto it = possible_solns.begin();
    string gen_hint;

    while (it != possible_solns.end()) {
        index = distance(possible_solns.begin(), it);

        gen_hint = check_code(cur_guess, possible_solns[index]);

        if (hint != gen_hint) {
            it = possible_solns.erase(possible_solns.begin() + index);
        } else {
            it++;
        }
    }
}

vector<vector<int>> min_max() {
    vector<vector<int>> optimal_guesses;
    map<string, int> hit_count;
    map<vector<int>, int> guess_scores;

    //for each unguessed code
    for (int i = 0; i < all_codes.size(); ++i) {
        vector<int> cur_guess = all_codes[i];

        //compare it to all possible solutions and record the hint pegs
        //each combination of hint pegs will have a corresponding number
        //stored in a map
        
        for (int j = 0; j < possible_solns.size(); ++j) {
            vector<int> variable_guess = possible_solns[j];

            string hint_pegs = check_code(all_codes[i], possible_solns[j]);
            if (hit_count.count(hint_pegs) > 0) {
                hit_count.at(hint_pegs)++;
            } else {
                hit_count.emplace(hint_pegs, 1);
            }
        }

        //get minimum elims
        //go through the hit_count map
        //the largest number results in the smallest possible number of eliminations
        //store these in a second map
        int max = 0;

        for (auto entry : hit_count) {
            if (entry.second > max) {
                max = entry.second;
            }
        }

        guess_scores.emplace(all_codes[i], max);
        hit_count.clear();
    }

    //get maximum of mins
    //go through guess_scores map
    //find highest value
    int min = INT_MAX;

    for (auto entry : guess_scores) {
        if (entry.second < min) {
            min = entry.second;
        }
    }

    //for every code with the highest value
    //store in the optimal_guess vector
    for (auto entry : guess_scores) {
        if (entry.second == min) {
            optimal_guesses.push_back(entry.first);
        }
    }

    //return the list of optimal guesses
    return optimal_guesses;
}

vector<int> generate_guess(vector<vector<int>> possible_solns, vector<vector<int>> all_codes) {
    //get optimal guesses
    vector<vector<int>> optimal_guesses = min_max();
    vector<int> guess;

    //play a guess from potential solution set if possible
    for (int i = 0; i < optimal_guesses.size(); ++i) {
        if (find(possible_solns.begin(), possible_solns.end(), optimal_guesses[i]) != possible_solns.end()) {
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

string print_guess(vector<int> guess) {
    //convert vector into string
    string result = "";
    for (int i = 0; i < NUM_DIGITS; i++) {
        result.append(to_string(guess[i]));
    }
    return result;
}

int main() {
    //initializations
    int turns = 1;
    vector<int> guess {1,1,2,2};
    bool won = false;
    create_sets();

    cout << "Mastermind Solver" << endl;
    cout << "Turn: " << turns << endl << "Guess: " << print_guess(guess) << endl;
    
    //game loop
    while (!won) {
        //check if hint input format is valid
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

        //if 4 colored pegs, game is won
        if (hint == "rrrr") {
            won = true;
            break;
        }

        //remove guess from possible solutions and future guesses
        remove_code(possible_solns, guess);
        remove_code(all_codes, guess);

        //remove from possible solutions any codes that do not match known hints
        trim_set(guess, hint);

        //pick the next guess
        guess = generate_guess(possible_solns, all_codes);

        //play the guess
        turns++;
        cout << "Turn: " << turns << endl << "Guess: " << print_guess(guess) << endl;

    }
    cout << "ggs! Stats: " << endl;
    cout << "guesses used: " << turns << endl;
    cout << "last guess chosen from " << possible_solns.size() << " possibilites" << endl;
    cout << "comparisons made: " << comparisons << endl;

    return 0;
}

