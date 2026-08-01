namespace tools {
    namespace maths {
        class Polynomial {
            public:
                int *polynomial = nullptr;
                int degree = 0;
                Polynomial() {}
                Polynomial(int *polynomial, int degree);
                ~Polynomial() {
                    delete polynomial;
                }
                Polynomial operator + (const Polynomial &other);
                Polynomial operator - (const Polynomial &other);
                Polynomial operator * (const Polynomial &other);
        };

        class ConvolutionRingPolynomial : public Polynomial {
            public:
                int maxdegree = 0;
                ConvolutionRingPolynomial() {}
                ConvolutionRingPolynomial(int *polynomial, int maxdegree);
                ConvolutionRingPolynomial operator + (const ConvolutionRingPolynomial &other);
                ConvolutionRingPolynomial operator - (const ConvolutionRingPolynomial &other);
                ConvolutionRingPolynomial operator * (const ConvolutionRingPolynomial &other);
        };
    }
}