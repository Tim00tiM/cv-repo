#include <cmath>
#include <vector>
#include <iostream>
#include <string>
#include <format>

namespace comp_results {
  enum class CompResult {
    LESS,
    EQUAL,
    GREAT
  };
}

constexpr long long pow(int a, size_t b) {
  long long ret_value = a;
  for (size_t i = 1; i < b; ++i) {
    ret_value *= a;
  }
  return ret_value;
}

using namespace comp_results;

class BigInteger {
 private:
  static const size_t kDigitSize = 6;
  static constexpr const long long kDigitMask = pow(10, kDigitSize);
  std::vector<long long> digits;
  size_t sz = 0;
  bool neg = false;

  long long AddValue(size_t index, long long value);

  long long SubstractDigit(size_t index, long long value);

  void CheckForZero();

  void Swap(BigInteger& swap);

  std::pair<BigInteger, BigInteger> SplitAt(size_t splitter) const;

  bool AbsLessForDivision(const BigInteger& to_comp, size_t offset) const;

  void ParseString(std::string num);

  BigInteger AbsValue();

  void AddTwoAbs(const BigInteger& add_value);

  static void SubstractSecondFromFirst(BigInteger& result, const BigInteger& sub);

  void Substract(const BigInteger& sub_value);

  void SubstractForDivision(const BigInteger& sub_value, size_t offset);

  CompResult Compare(const BigInteger& comp_value) const;

  std::pair<BigInteger, BigInteger> Divide(BigInteger divisor);

 public:
  BigInteger() {};

  BigInteger(long long value) {
    if (value == 0) {
      sz += 1;
      digits.push_back(0);
      return;
    }
    if (value < 0) {
      neg = true;
      value = -value;
    }
    while (value / kDigitMask != 0) {
      digits.push_back(value % kDigitMask);
      value /= kDigitMask;
      sz += 1;
    }
    if (value != 0) {
      sz += 1;
    }
    digits.push_back(value);
  }

  BigInteger(const BigInteger& to_copy) : digits(to_copy.digits), sz(to_copy.sz), neg(to_copy.neg) {}

  BigInteger(std::string num) {
    size_t offset;
    long long digit;
    size_t num_size = num.size();
    if (num.find("e") == std::variant_npos) {
      for (size_t i = 0; i < num_size; i+=kDigitSize) {
        std::string add = std::string(kDigitSize, '0');
        offset = 0;
        while (i + offset < num_size && offset < kDigitSize) {
          if (num[num_size - i - offset - 1] == '-') {
            neg = true;
            break;
          }
          add[kDigitSize - offset - 1] = num[num_size - i - offset - 1];
          offset += 1;
        }
        digit = std::stoll(add);
        digits.push_back(digit);
        sz += 1;
      }
      CheckForZero();
    } else {
      int remain = std::stoi(num.substr(5));
      digits.assign(remain / 6, 0);
      std::string leading_numbers;
      std::string next_numbers;
      if (remain % 6 > 1) {
        leading_numbers = num.substr(0, 1) + num.substr(2, 2) + std::string(remain % 6 - 2, '0');
        next_numbers = "0";
      }
      if (remain % 6 == 1) {
        leading_numbers = num.substr(0, 1) + num.substr(2, 1);
        next_numbers = num.substr(3, 1) + "00000";
      }
      if (remain % 6 == 0) {
        leading_numbers = num.substr(0, 1);
        next_numbers = num.substr(2, 2) + "0000";
      }
      digits[digits.size() - 1] = std::stoll(next_numbers);
      digits.push_back(std::stoll(leading_numbers));
      sz = digits.size();
    }
  }

  size_t GetSize() const;

  BigInteger& operator+=(const BigInteger& add_value);

  BigInteger operator-() const;

  BigInteger& operator-=(const BigInteger& add_value);

  BigInteger& operator*=(const BigInteger& mul_value);

  BigInteger& operator/=(const BigInteger& divisor);

  BigInteger& operator%=(const BigInteger& divisor);

  BigInteger& operator=(BigInteger copy);

  std::string toString() const;

  BigInteger& operator++();

  BigInteger operator++(int);

  BigInteger& operator--();

  BigInteger operator--(int);

  explicit operator bool() const;

  friend bool operator==(const BigInteger& first, const BigInteger& second);
  friend bool operator<(const BigInteger& first, const BigInteger& second);
  friend BigInteger operator*(const BigInteger& first, const BigInteger& second);
  friend std::istream& operator>>(std::istream& in, BigInteger& bigInt);
};

BigInteger operator+(const BigInteger& first, const BigInteger& second);

BigInteger operator-(const BigInteger& first, const BigInteger& second);

BigInteger operator*(const BigInteger& first, const BigInteger& second);

std::ostream& operator<<(std::ostream& out, const BigInteger& bigInt);

BigInteger operator/(const BigInteger& first, const BigInteger& second);

BigInteger operator%(const BigInteger& first, const BigInteger& second);

bool operator<(const BigInteger& first, const BigInteger& second);

bool operator==(const BigInteger& first, const BigInteger& second);

bool operator>(const BigInteger& first, const BigInteger& second);

bool operator<=(const BigInteger& first, const BigInteger& second);

bool operator>=(const BigInteger& first, const BigInteger& second);

bool operator!=(const BigInteger& first, const BigInteger& second);

std::istream& operator>>(std::istream& in, BigInteger& bigInt);

BigInteger operator""_bi(const char* init, long unsigned int);