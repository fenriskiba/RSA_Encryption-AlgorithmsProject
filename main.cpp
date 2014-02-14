#include <iostream>
#include <cmath>
#include <bitset>
#include <string>
#include <sstream>
#include "BigInt.h"

using namespace std;

BigInt randomPrime(BigInt size); //Generates a prime number with 'size' digits
BigInt* euclideanAlgorithm(BigInt a, BigInt b); //Outputs a 3 element array with elements (d,x,y) such that gcd(a,b) = ax+by and y>0
BigInt* modInverse(BigInt e, BigInt p, BigInt q); //Outputs a 2 element array with elements (d,n) such that ed=1%(p-1)(q-1), n=pq
string rsaEncrypt(BigInt e, BigInt n, string message); //Outputs the RSA Encrypted message
string rsaDecrypt(BigInt d, BigInt n, string eMessage); //Decrypts and outputs the encrypted message

BigInt generateRandom(BigInt size); //Generates a random number of the given size
BigInt bigPow(BigInt a, BigInt b); //Calculates a^b
BigInt modularExponentiation(BigInt base, BigInt exp, BigInt mod); //Calculates a^b mod x
bool fermatsLittleTheorem(BigInt possiblePrime);
string* stringToAscii(string message); //Converts a string to an array of numbers representing the ascii values
string* asciiBlocks(string* asciiMessage, int& length);
BigInt* asciiToDecimal(string* asciiMessage, int length);

int main(int argc, char *argv[])
{
    srand(time(0));
    
    if(argc == 1 || argc > 5)
    {
        cout << "Wrong input format!" << endl;
    }
    else if(argc == 2)
    {
        BigInt size = atoi(argv[1]);
        cout << "Size of your prime number: " << size << endl;
        cout << "Your prime number is: " << randomPrime(size) << endl;
    }
    else if(argc == 3)
    {
        BigInt a = atoi(argv[1]);
        BigInt b = atoi(argv[2]);
        cout << "a = " << a << ", b = " << b << endl;
        BigInt* euclidean = euclideanAlgorithm(a,b);
        cout << "x = " << euclidean[1] << ", y = " << euclidean[2] << endl;
        delete [] euclidean;
    }
    else if(argc == 4)
    {
        BigInt e = atoi(argv[1]);
        BigInt p = atoi(argv[2]);
        BigInt q = atoi(argv[3]);
        cout << "e = " << e << ", p = " << p <<", q = "<< q << endl;
        BigInt* mods = modInverse(e, p, q);
        cout << "d = " << mods[0] << ", n = " << mods[1] << endl;
    }
    else if(argc == 5)
    {
        if(argv[1][0] == 'e')
        {
            BigInt e = atoi(argv[2]);
            BigInt n = atoi(argv[3]);
            string message = argv[4];
            cout << "e = " << e << ", n = " << n <<", message = \""<< message << "\"" << endl;
            string encrypted = rsaEncrypt(e, n, message);
            cout << "The encrypted message is: " << encrypted << endl;
        }
        else if(argv[1][0]=='d')
        {
            BigInt d = atoi(argv[2]);
            BigInt n = atoi(argv[3]);
            string message = argv[4];
            cout << "d = " << d << ", n = " << n <<", message = \""<< message << "\"" << endl;
            string decrypted = rsaDecrypt(d, n, message);
            cout << "The decrypted message is: " << decrypted << endl;
        }
    }
    
    return 0;
}

BigInt randomPrime(BigInt size)
{
	BigInt possiblePrime = generateRandom(size);
    
    while(!fermatsLittleTheorem(possiblePrime))
        possiblePrime = generateRandom(size);

	return possiblePrime;
}

BigInt generateRandom(BigInt size)
{
    BigInt result = 0;
    BigInt digit;
    
    for(BigInt i = 0; i < size - 1; i = i + 1)
    {
        digit = rand();
        result = result + (digit % 10) * bigPow(10, i);
    }
    
    digit = rand();
    
    result = result + ((digit % 9) + 1) * bigPow(10, size - 1);
    
    return result;
}

BigInt bigPow(BigInt base, BigInt exponent)
{
    if (exponent == 0) 
    {
        return 1;
    }
    else if (exponent == 1)
    {
        return base;
    }
    else if (exponent % 2 == 0)
    {
        BigInt t = bigPow(base, exponent / 2);
        return t * t;
    }
    else
    {
        BigInt t = bigPow(base, exponent / 2);
        return t * t * base;
    }
}

BigInt modularExponentiation(BigInt base, BigInt exp, BigInt mod)
{
    if(exp == 0)
    {
        return 1;
    }
    else
    {
        BigInt z = modularExponentiation(base, exp / 2, mod);
        
        if((exp % 2) == 0)
        {
            return (z * z % mod);
        }
        else
        {
            return (base * z * z % mod);
        }
    }
}

bool fermatsLittleTheorem(BigInt possiblePrime)
{
    BigInt random;
    int a = 2; //Increase this number to improve reliability
    
    for(int i = 0; i < a; i++)
    {
        random = rand();
        random = random % (possiblePrime - 1) + 1;
        random = modularExponentiation(random, possiblePrime - 1, possiblePrime);
        if(random != 1)
            return false;
    }
    
    return true;
}

BigInt* euclideanAlgorithm(BigInt a, BigInt b)
{
    BigInt* result = new BigInt[3];
    BigInt* resultPrime;
    
    if(b == 0)
    {
        result[0] = a;
        result[1] = 1;
        result[2] = 0;
        return result;
    }
    
    resultPrime = euclideanAlgorithm(b, a % b);
    
    result[0] = resultPrime[0];
    result[1] = resultPrime[2];
    result[2] = resultPrime[1] - ((a / b) * resultPrime[2]);
    
    delete [] resultPrime;
    
    return result;
}

BigInt* modInverse(BigInt e, BigInt p, BigInt q)
{
    BigInt n = p * q;
    BigInt m = (p - 1) * (q - 1);
    BigInt* dTemp = euclideanAlgorithm(e,m);
    BigInt d = (dTemp[1] + m) % m;
    
    BigInt* result = new BigInt[2];
    result[0] = d;
    result[1] = n;
    
    return result;
}

string rsaEncrypt(BigInt e, BigInt n, string message)
{
    string* asciiMessage = stringToAscii(message);
    
    int numOfBlocks = message.length();
    string* combinedBlocks = asciiBlocks(asciiMessage, numOfBlocks);
    
    delete [] asciiMessage;
    BigInt* toBeEncrypted = asciiToDecimal(combinedBlocks, numOfBlocks);
    
    delete [] combinedBlocks;
    
    return "meh";
}

string* stringToAscii(string message)
{
    string* result = new string[message.length()];
    
    for(int i = 0; i < message.length(); i++)
    {
        stringstream temp;
        temp << bitset<8>(message.c_str()[i]);
        temp >> result[i];
    }
    
    return result;
}

string* asciiBlocks(string* asciiMessage, int& length)
{
    bool isEven = length % 2 ? false : true;
    int newLength = (isEven ? length / 2 : (length / 2) + 1);
    string* result = new string[newLength];
    int asciiIndex = 0;
    
    for(int i = 0; i < (length / 2); i++)
    {
        result[i] = asciiMessage[asciiIndex] + asciiMessage[asciiIndex + 1];
        asciiIndex += 2;
    }
    
    if(!isEven)
    {
        result[newLength - 1] = "00000000" + asciiMessage[length - 1];
    }
    
    length = newLength;
        
    return result;
}

BigInt* asciiToDecimal(string* binary, int length)
{
    unsigned long decimal = 0;
    BigInt* result = new BigInt[length];
    
    for(int i = 0; i < length; i++)
    {
        decimal = bitset<32>(binary[i]).to_ulong();
        result[i] = (int) decimal;
    }
    
    return result;
}

string rsaDecrypt(BigInt d, BigInt n, string eMessage)
{
    return "testing123";
}

