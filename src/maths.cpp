#include "maths.h"
namespace tools {
    namespace maths {
        Polynomial::Polynomial(int *polynomial, int degree) {
            this->polynomial = polynomial;
            this->degree = degree;
        }

        Polynomial Polynomial::operator + (const Polynomial &other) {
            int maxpoly = 0;
            for (int i = this->degree; i > 0; i++) {
                if (this->polynomial[i] != 0 || other.polynomial[i] != 0) {
                    maxpoly = i;
                    break;
                }
            }
            int newmax = maxpoly;
            int* newpoly = new int[newmax];
            for (int i = 0; i < newmax; i++) {
                int tmp = 0;
                if (this->degree > i) {
                    tmp += this->polynomial[i];
                }
                if (other.degree > i) {
                    tmp += other.polynomial[i];
                }
                newpoly[i] = tmp;
            }
            return Polynomial(newpoly, newmax);
        }

        Polynomial Polynomial::operator - (const Polynomial &other) {
            int maxpoly = 0;
            for (int i = this->degree; i > 0; i++) {
                if (this->polynomial[i] != 0 || other.polynomial[i] != 0) {
                    maxpoly = i;
                    break;
                }
            }
            int newmax = maxpoly;
            int* newpoly = new int[newmax];
            for (int i = 0; i < newmax; i++) {
                int tmp = 0;
                if (this->degree > i) {
                    tmp += this->polynomial[i];
                }
                if (other.degree > i) {
                    tmp -= other.polynomial[i];
                }
                newpoly[i] = tmp;
            }
            return Polynomial(newpoly, newmax);
        }

        Polynomial Polynomial::operator * (const Polynomial &other) {
            int maxpolyme = 0;
            int maxpolyother = 0;
            for (int i = this->degree; i > 0; i++) {
                if (this->polynomial[i] != 0) {
                    maxpolyme = i;
                    break;
                }
            }
            for (int i = other.degree; i > 0; i++) {
                if (other.polynomial[i] != 0) {
                    maxpolyother = i;
                    break;
                }
            }
            int newmax = maxpolyme * maxpolyother;
            int* newpoly = new int[newmax];
            for (int i = 0; i < maxpolyme; i++) {
                for (int t = 0; t < maxpolyother; t++) {
                    newpoly[i + t] = this->polynomial[i] * other.polynomial[t];
                }
            }
            return Polynomial(newpoly, newmax);
        }

        ConvolutionRingPolynomial::ConvolutionRingPolynomial(int *polynomial, int maxdegree) {
            this->polynomial = new int[maxdegree];
            this->maxdegree = maxdegree;
            this->degree = maxdegree;
        }

        ConvolutionRingPolynomial ConvolutionRingPolynomial::operator + (const ConvolutionRingPolynomial &other) {
            int newmax = this->maxdegree;
            int* newpoly = new int[newmax];
            for (int i = 0; i < newmax; i++) {
                newpoly[i] = this->polynomial[i] + other.polynomial[i];
            }
            return ConvolutionRingPolynomial(newpoly, newmax);
        }

        ConvolutionRingPolynomial ConvolutionRingPolynomial::operator - (const ConvolutionRingPolynomial &other) {
            int newmax = this->maxdegree;
            int* newpoly = new int[newmax];
            for (int i = 0; i < newmax; i++) {
                newpoly[i] = this->polynomial[i] - other.polynomial[i];
            }
            return ConvolutionRingPolynomial(newpoly, newmax);
        }

        ConvolutionRingPolynomial ConvolutionRingPolynomial::operator * (const ConvolutionRingPolynomial &other) {
            int newmax = this->maxdegree;
            int* newpoly = new int[newmax];
            for (int i = 0; i < newmax; i++) {
                for (int t = 0; t < newmax; t++) {
                    newpoly[(i + t) % newmax] = this->polynomial[i] * other.polynomial[t];
                }
            }
            return ConvolutionRingPolynomial(newpoly, newmax);
        }
    }
}