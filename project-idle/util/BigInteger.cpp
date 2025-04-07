
#include "BigInteger.h"


std::string Format(long long zeros, long long number) {
  std::string ret;
  ret += std::to_string(number);
  std::string add;
  for (size_t i = 0; i < zeros - ret.size(); ++i) {
    add += "0";
  }
  ret = add + ret;
  return ret;
}

long long BigInteger::AddValue(size_t index, long long value) {
  digits[index] += value;
  long long ret = digits[index] / kDigitMask;
  digits[index] = digits[index] % kDigitMask;
  return ret;
}

long long BigInteger::SubstractDigit(size_t index, long long value) {
  digits[index] -= value;
  long long ret_value = 0;
  if (digits[index] < 0) {
    ret_value = 1;
    digits[index] += kDigitMask;
  }
  return ret_value;
}

void BigInteger::CheckForZero() {
  while (sz > 0 && digits[sz - 1] == 0) {
    digits.pop_back();
    sz -= 1;
  }
  if (sz == 0) {
    *this = BigInteger(0);
  }
  digits.shrink_to_fit();
}

void BigInteger::Swap(BigInteger& swap) {
  std::swap(digits, swap.digits);
  std::swap(sz, swap.sz);
  std::swap(neg, swap.neg);
}

std::pair<BigInteger, BigInteger> BigInteger::SplitAt(size_t splitter) const {
  BigInteger high;
  BigInteger low;
  for (size_t i = 0; i < splitter && i < sz; ++i) {
    low.digits.push_back(digits[i]);
    low.sz += 1;
  }
  low.CheckForZero();
  for (size_t i = splitter; i < sz; ++i) {
    high.digits.push_back(digits[i]);
    high.sz += 1;
  }
  if (high.sz == 0) {
    high = 0;
  }
  high.CheckForZero();
  return {high, low};
}

bool BigInteger::AbsLessForDivision(const BigInteger& to_comp, size_t offset) const {
  if (sz - offset == to_comp.sz) {
    for (size_t i = sz - offset; i--;) {
      if (digits[i + offset] < to_comp.digits[i]) {
        return true;
      }
      if (digits[i + offset] > to_comp.digits[i]) {
        return false;
      }
    }
    return false;
  }
  return to_comp.sz > sz - offset;
}

void BigInteger::ParseString(std::string num) {
  size_t offset;
  long long digit;
  size_t num_size = num.size();
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
}

BigInteger BigInteger::AbsValue() {
  BigInteger copy = *this;
  if (copy.neg) {
    copy.neg = false;
  }
  return copy;
}

void BigInteger::AddTwoAbs(const BigInteger& add_value) {
  long long carry_one = 0;
  size_t i;
  if (add_value.GetSize() > GetSize()) {
    size_t diff = add_value.GetSize() - GetSize();
    for (i = 0; i < diff; ++i) {
      digits.push_back(0);
      sz += 1;
    }
  }
  for (i = 0; i < add_value.GetSize(); ++i) {
    carry_one = AddValue(i, add_value.digits[i] + carry_one);
  }
  while (carry_one != 0) {
    if (i == GetSize()) {
      sz += 1;
      digits.push_back(0);
    }
    carry_one = AddValue(i, carry_one);
    ++i;
  }
}

void BigInteger::SubstractSecondFromFirst(BigInteger& result, const BigInteger& sub) {
  size_t i;
  long long carry_one = 0;
  for (i = 0; i < sub.sz; ++i) {
    carry_one = result.SubstractDigit(i, sub.digits[i] + carry_one);
  }
  while (carry_one != 0) {
    carry_one = result.SubstractDigit(i, carry_one);
    ++i;
  }
}

void BigInteger::Substract(const BigInteger& sub_value) {
  BigInteger result;
  if (Compare(sub_value) == CompResult::LESS) {
    result = sub_value;
    SubstractSecondFromFirst(result, *this);
  } else {
    result = *this;
    SubstractSecondFromFirst(result, sub_value);
  }
  *this = result;
}

void BigInteger::SubstractForDivision(const BigInteger& sub_value, size_t offset) {
  size_t i;
  long long carry_one = 0;
  for (i = 0; i < sub_value.sz; ++i) {
    carry_one = SubstractDigit(offset + i, sub_value.digits[i] + carry_one);
  }
  while (carry_one != 0) {
    carry_one = SubstractDigit(offset + i, carry_one);
    ++i;
  }
  CheckForZero();
}

CompResult BigInteger::Compare(const BigInteger& comp_value) const {
  if (sz < comp_value.sz) {
    return CompResult::LESS;
  }
  if (sz > comp_value.sz) {
    return CompResult::GREAT;
  }
  for (size_t i = sz; i--;) {
    if (digits[i] < comp_value.digits[i]) {
      return CompResult::LESS;
    }
    if (digits[i] > comp_value.digits[i]) {
      return CompResult::GREAT;
    }
  }
  return CompResult::EQUAL;
}

std::pair<BigInteger, BigInteger> BigInteger::Divide(BigInteger divisor) {
  if (Compare(divisor) == CompResult::LESS) {
    return {0, *this};
  }
  bool start_neg = neg;
  neg = false;
  divisor.neg = false;
  BigInteger result(0);
  BigInteger sub;
  BigInteger compare;
  long long dividend;
  long long delim = divisor.digits[divisor.sz - 1];
  size_t delim_length = 1;
  if (divisor.sz > 1) {
    delim_length += 1;
    delim *= kDigitMask;
    delim += divisor.digits[divisor.sz - 2];
  }
  sub = divisor;
  for (size_t i = sz - divisor.sz + delim_length; i--;) {
    if (i + divisor.sz - delim_length + 1 > sz) {
      result *= kDigitMask;
      continue;
    }
    dividend = 0;
    if (i + divisor.sz + 1 - (delim_length - 1) <= sz) {
      if (i + divisor.sz + 2 - (delim_length - 1) <= sz) {
        dividend = digits[i + divisor.sz - delim_length + 2];
      }
      dividend *= kDigitMask;
      dividend += digits[i + divisor.sz - delim_length + 1];
    }
    dividend *= kDigitMask;
    dividend += digits[i + divisor.sz - delim_length];
    dividend /= delim;
    if (dividend <= 0) {
      continue;
    }
    compare = BigInteger(sub * dividend);
    if (AbsLessForDivision(compare, i)) {
      dividend -= 1;
      compare -= sub;
    }
    result += dividend;
    if (i != 0) {
      result *= kDigitMask;
    }
    SubstractForDivision(compare, i);
    if (*this == 0) {
      if (i != 0) {
        for (size_t j = i - 1; j--;) {
          result *= kDigitMask;
        }
      }
      break;
    }  //  end of for
  }
  neg = start_neg;
  return {result, *this};
}

size_t BigInteger::GetSize() const {
  return sz;
}

BigInteger& BigInteger::operator+=(const BigInteger& add_value) {
  if (!(neg xor add_value.neg)) {
    AddTwoAbs(add_value);
  } else {
    Substract(add_value);
  }
  CheckForZero();
  return *this;
}

BigInteger BigInteger::operator-() const {
  BigInteger copy = *this;
  copy.neg = !copy.neg;
  return copy;
}

BigInteger& BigInteger::operator-=(const BigInteger& add_value) {
  bool final_neg = neg;
  if (Compare(add_value) == CompResult::LESS) {
    final_neg = !add_value.neg;
  }
  if (!(neg xor add_value.neg)) {
    Substract(add_value);
  } else {
    AddTwoAbs(add_value);
  }
  neg = final_neg;
  CheckForZero();
  return *this;
}

BigInteger& BigInteger::operator*=(const BigInteger& mul_value) {
  *this = *this * mul_value;
  return *this;
}

BigInteger& BigInteger::operator/=(const BigInteger& divisor) {
  std::pair<BigInteger, BigInteger> result = Divide(divisor);
  if (neg xor divisor.neg) {
    result.first.neg = true;
  }
  *this = result.first;
  CheckForZero();
  return *this;
}

BigInteger& BigInteger::operator%=(const BigInteger& divisor) {
  std::pair<BigInteger, BigInteger> result = Divide(divisor);
  if (neg) {
    result.second.neg = true;
  }
  *this = result.second;
  CheckForZero();
  return *this;
}

BigInteger& BigInteger::operator=(BigInteger copy) {
  Swap(copy);
  return *this;
}

std::string BigInteger::toString() const {
  std::string ret;
  if (digits.size() > 1) {
    std::string to_exp = std::to_string(digits[GetSize() - 1] * kDigitMask + digits[GetSize() - 2]);
    ret = std::format("{0}.{1}{2}e{3}", to_exp[0], to_exp[1], to_exp[2], to_exp.size() + (digits.size() - 2) * kDigitSize - 1);
    return ret;
  }
  if (neg && digits[GetSize() - 1] != 0) {
    ret.append("-");
  }
  ret.append(std::to_string(digits[GetSize() - 1]));
  for (size_t i = GetSize() - 1; i--;) {
    std::string add = Format(kDigitSize, digits[i]);
    ret.append(add);
  }
  return ret;
}

BigInteger& BigInteger::operator++() {
  *this += 1;
  return *this;
}

BigInteger BigInteger::operator++(int) {
  BigInteger ret = *this;
  *this += 1;
  return ret;
}

BigInteger& BigInteger::operator--() {
  *this -= 1;
  return *this;
}

BigInteger BigInteger::operator--(int) {
  BigInteger ret = *this;
  *this -= 1;
  return ret;
}

BigInteger operator+(const BigInteger& first, const BigInteger& second) {
  BigInteger val = first;
  val += second;
  return val;
}

BigInteger operator-(const BigInteger& first, const BigInteger& second) {
  BigInteger val = first;
  val -= second;
  return val;
}

BigInteger operator*(const BigInteger& first, const BigInteger& second) {
  BigInteger equal = 0;
  for (size_t i = 0; i < first.GetSize() + second.GetSize(); ++i) {
    equal.digits.push_back(0);
    equal.sz += 1;
  }
  long long carry;
  long long k;
  for (size_t i = 0; i < second.GetSize(); ++i) {
    for (size_t j = 0; j < first.GetSize(); ++j) {
      carry = second.digits[i] * first.digits[j];
      k = 0;
      while (carry != 0) {
        carry = equal.AddValue(i + j + k, carry);
        ++k;
      }
    }
  }
  equal.neg = second.neg xor first.neg;
  equal.CheckForZero();
  return equal;
}

std::ostream& operator<<(std::ostream& out, const BigInteger& bigInt) {
  out << bigInt.toString();
  return out;
}

BigInteger operator/(const BigInteger& first, const BigInteger& second) {
  BigInteger val = first;
  val /= second;
  return val;
}

BigInteger operator%(const BigInteger& first, const BigInteger& second) {
  BigInteger val = first;
  val %= second;
  return val;
}

bool operator<(const BigInteger& first, const BigInteger& second) {
  if (first.neg and !second.neg) {
    return true;
  }
  if (!first.neg and second.neg) {
    return false;
  }
  if (first.neg) {
    return second.Compare(first) == CompResult::LESS;
  }
  return first.Compare(second) == CompResult::LESS;
}

bool operator==(const BigInteger& first, const BigInteger& second) {
  if (first.neg == second.neg) {
    return first.Compare(second) == CompResult::EQUAL;
  }
  return false;
}

bool operator>(const BigInteger& first, const BigInteger& second) {
  return second < first;
}

bool operator<=(const BigInteger& first, const BigInteger& second) {
  return !(first > second);
}

bool operator>=(const BigInteger& first, const BigInteger& second) {
  return !(first < second);
}

bool operator!=(const BigInteger& first, const BigInteger& second) {
  return !(first == second);
}

BigInteger::operator bool() const {
  return (*this != BigInteger(0));
};

std::istream& operator>>(std::istream& in, BigInteger& bigInt) {
  std::string num;
  in >> num;
  bigInt = BigInteger(num);
  return in;
}

BigInteger operator""_bi(const char* init, long unsigned int) {
  std::string num = init;
  BigInteger bigInt = BigInteger(init);
  return bigInt;
}