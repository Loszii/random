#include <iostream>
#include <chrono>
using namespace std;

string add(const string& a, const string& b) {
    //adds two strings and returns the result as a string
    //edge case
    if (a.empty()) {
        return b;
    } else if (b.empty()) {
        return a;
    }

    //set up num1 and num2
    string num1 = a;
    string num2 = b;

    //remove any leading zeroes
    while (!num1.empty() && num1.front() == '0') {
        if (num1.size() == 1) {
            break; //keep last 0
        }
        num1.erase(num1.begin());
    }
    while (!num2.empty() && num2.front() == '0') {
        if (num2.size() == 1) {
            break; //keep last 0
        }
        num2.erase(num2.begin());
    }

    if (num1.size() < num2.size()) { //num1 be bigger number
        string temp = num1; //swap
        num1 = num2;
        num2 = temp;
    }

    int num1_len = num1.size(); //iterate over bigger number
    int num2_len = num2.size();
    string result;
    result.reserve(num1_len); //will at least be size len so can reserve mem
    int carry = 0;

    //loop starting at end (len-1)
    for (int i=1; i <= num1_len; i++) {
        int digit_1 = num1[num1_len - i] - '0'; //will always be there
        int digit_2; //must check since smaller number
        if (num2_len - i >= 0) {
            digit_2 = num2[num2_len - i] - '0';
        } else { //out of bound in smaller num so just use 0
            digit_2 = 0;
        }

        int sum = digit_1 + digit_2 + carry; //sum them
        if (sum > 9) { //apply carry / calculate carry
            carry = 1;
            sum -= 10;
        } else {
            carry = 0;
        }

        result.push_back(sum + '0'); //add to end since faster, will reverse at end
    }

    if (carry) { //add final carry
        result.push_back('1');
    }

    result = string(result.rbegin(), result.rend()); //reverse
    return result;
}

string subtract(const string& a, const string& b) {
    //subtract two strings and returns the absolute value as a string (doesnt work with negative)
    //edge case
    if (a.empty()) {
        return b;
    } else if (b.empty()) {
        return a;
    }

    //initialize both numbers
    string num1 = a;
    string num2 = b;
    //remove any leading zeroes
    while (!num1.empty() && num1.front() == '0') {
        if (num1.size() == 1) {
            break; //keep last 0
        }
        num1.erase(num1.begin());
    }
    while (!num2.empty() && num2.front() == '0') {
        if (num2.size() == 1) {
            break; //keep last 0
        }
        num2.erase(num2.begin());
    }

    if (num1.size() < num1.size()) { //make larger number on top
        string temp = num1; //swap num1 and num2
        num1 = num2;
        num2 = temp;
    } else if (num1.size() == num2.size()) { //if equal compare the first non-equal digit
        for (size_t i=0; i < num1.size(); i++) {
            if (num2[i] > num1[i]) {
                string temp = num1; //swap
                num1 = num2;
                num2 = temp;
                break;
            } else if (num2[i] < num1[i]) {
                break; //already set to right values
            }
        }
    }

    int num1_len = num1.size();
    int num2_len = num2.size();
    string result;
    result.reserve(num1_len);

    for (int i=1; i <= num1_len; i++) {
        int difference;
        if (num2_len - i >= 0) {
            difference = num1[num1_len - i] - num2[num2_len - i]; //take difference
        } else {
            difference = num1[num1_len - i] - '0'; //treat num2 as a leading 0
        }

        if (difference < 0) { //need to borrow
            num1[num1_len - i - 1] -= 1; //subtract 1 from pos to left (safe since num1[0] > num2[0])
            difference += 10; //add from borrowed quantity
        }
        result.push_back(difference + '0');
    }

    //remove any leading zeros (trailing since have yet to reverse)
    while (!result.empty() && result.back() == '0') {
        if (result.size() == 1) {
            break; //keep last 0
        }
        result.pop_back();
    }
    result = string(result.rbegin(), result.rend()); //reverse
    return result;
}

string multiply(const string& a, const string& b) {
    //edge case (should never happen since we never remove final leading 0 but just for segmentation fault security)
    if (a.empty() || b.empty()) {
        return "0"; //assume one is 0 so product is zero
    }

    string num1 = a; //make new so we can modify (const reference)
    string num2 = b;
    //pad with zeroes
    while (num1.size() < num2.size()) {
        num1.insert(num1.begin(), '0');
    }
    while (num2.size() < num1.size()) {
        num2.insert(num2.begin(), '0');
    }

    int len = num1.size(); //same len for both
    int half = len/2; //if even this will make the upper half smaller (rounded down)
    int remaining = len - half; //compute size of lower half
    string result;
    result.reserve(len);

    if (len == 1) { //base case just multiply two single digits and return
        int digit_1 = num1[0] - '0';
        int digit_2 = num2[0] - '0';
        int product = digit_1 * digit_2;
        while (product != 0) { //move over to result string
            int cur = product % 10; //get last digit in product
            result.push_back(cur + '0'); //add to result string
            product = product / 10; //move over
        }

        if (result.empty()) { //product was 0
            return "0";
        } else {
            result = string(result.rbegin(), result.rend()); //reverse since added to end
            return result;
        }
    }

    //split both into two
    string a1 = num1.substr(0, half);
    string a0 = num1.substr(half, remaining);
    string b1 = num2.substr(0, half);
    string b0 = num2.substr(half, remaining);

    //3 recursive calls
    string c0 = multiply(a0, b0); //a0 * b0
    string c2 = multiply(a1, b1); //a1 * b1
    string c1 = subtract(subtract(multiply(add(a1, a0), add(b1, b0)), c2), c0); //(a1 + a0)(b1 + b0) - (c2 + c0)

    //shift c2
    int to_shift = remaining * 2; //n (using remaining to handle when n is odd)
    while (to_shift) {
        c2.push_back('0');
        to_shift -= 1;
    }

    //shift c1
    to_shift = remaining; //n/2
    while (to_shift) {
        c1.push_back('0');
        to_shift -= 1;
    }

    result = add(c2, add(c1, c0)); //this will remove leading zeros
    return result;
}

int main(int argc, char* argv[]) {
    //get cmd args
    if (argc != 3) {
        cerr << "Usage: ./fastmult <int> <int>" << endl;
        return 1;
    }
    string num1 = argv[1];
    string num2 = argv[2];
    //clock_t start = clock();
    cout << multiply(num1, num2) << endl;
    /*
    clock_t end = clock();
    double total_time = double(end - start) / double(CLOCKS_PER_SEC);
    cout << "time: " << total_time << endl;*/
    return 0;
}