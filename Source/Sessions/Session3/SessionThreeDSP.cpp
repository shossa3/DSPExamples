/*
  ==============================================================================

  Shaikat Hossain DSPExamples

  ==============================================================================
*/

#include "../Session1/SessionOneDSP.h"
#include "../Session2/SessionTwoDSP.h"
#include "SessionThreeDSP.h"

//==============================================================================
const void SessionThreeDSP::calculusLP2Coefficients (float frequency, float Q,
    int algorithm, double _sampleRate,
    int &order,
    float &b0, float &b1, float &b2, float &b3, float &b4,
    float &a1, float &a2, float &a3, float &a4)
{
    auto Ts = 1.0 / _sampleRate;
    auto w0 = 2.0 * double_Pi * frequency;
    
    if (algorithm == Algorithm_BLT_NoWarping || algorithm == Algorithm_BLT_Warping || algorithm == Algorithm_Trap)
    {
        if (algorithm == Algorithm_BLT_Warping)
            Ts = std::tan (w0 * Ts / 2.0) * 2.0 / w0;

        auto Ts2 = Ts * Ts;
        auto w02 = w0 * w0;

        auto a0 = Q * Ts2 * w02 + 2 * Ts * w0 + 4 * Q;

        b0 = (float)((Q * Ts2 * w02) / a0);
        b1 = (float)((2 * Q * Ts2 * w02) / a0);
        b2 = (float)((Q * Ts2 * w02) / a0);

        a1 = (float)((2 * Q * Ts2 * w02 - 8 * Q) / a0);
        a2 = (float)((Q * Ts2 * w02 - 2 * Ts * w0 + 4 * Q) / a0);

        b3 = 0.f; b4 = 0.f;
        a3 = 0.f; a4 = 0.f;

        order = 2;
    }
    else if (algorithm == Algorithm_Forward_Euler)
    {
        auto Ts2 = Ts * Ts;
        auto w02 = w0 * w0;

        auto a0 = Q;

        b0 = 0.f;
        b1 = 0.f;
        b2 = (float)((Q * Ts2 * w02) / a0);

        a1 = (float)((Ts * w0 - 2 * Q) / a0);
        a2 = (float)((Q - Ts * w0 + Q * Ts2 * w02) / a0);

        b3 = 0.f; b4 = 0.f;
        a3 = 0.f; a4 = 0.f;

        order = 2;
    }
    else if (algorithm == Algorithm_Backward_Euler)
    {
        auto Ts2 = Ts * Ts;
        auto w02 = w0 * w0;

        auto a0 = Q + Ts * w0 + Q * Ts2 * w02;

        b0 = (float)((Q * Ts2 * w02) / a0);
        b1 = 0.f;
        b2 = 0.f;        

        a1 = (float)((-Ts * w0 - 2 * Q) / a0);
        a2 = (float)(Q / a0);

        b3 = 0.f; b4 = 0.f;
        a3 = 0.f; a4 = 0.f;

        order = 2;
    }
    else if (algorithm == Algorithm_BDF2)
    {
        auto Ts2 = Ts * Ts;
        auto w02 = w0 * w0;

        auto a0 = 4 * Q * Ts2 * w02 + 6 * Ts * w0 + 9 * Q;

        b0 = (float)((4 * Q * Ts2 * w02) / a0);
        b1 = 0.f;
        b2 = 0.f;
        b3 = 0.f;
        b4 = 0.f;

        a1 = (float)((-24 * Q - 8 * Ts * w0) / a0);
        a2 = (float)((22 * Q + 2 * Ts * w0) / a0);
        a3 = (float)(-8*Q / a0);
        a4 = (float)(Q / a0);

        order = 4;
    }
    else if (algorithm == Algorithm_ImpulseInvariance)
    {
        float b0A, b1A, b2A, a0A, a1A, a2A;
        SessionTwoDSP::calculusAnalogCoefficients (frequency, Q, 0.f, SessionTwoDSP::FilterType::Type_LowPass2,
            b0A, b1A, b2A, a1A, a2A);

        a0A = 1.f;
        b0A /= a2A; b1A /= a2A; b2A /= a2A;
        a0A /= a2A; a1A /= a2A; a2A /= a2A;

        std::complex<double> delta = a1A * a1A - 4 * a0A * a2A;
        std::complex<double> s1 = (std::complex<double> (-a1A) - std::sqrt (delta)) / std::complex<double> (2 * a2A);
        std::complex<double> s2 = (std::complex<double> (-a1A) + std::sqrt (delta)) / std::complex<double> (2 * a2A);
        auto B0 = std::complex<double> (b0A - a0A * b2A);
        auto B1 = std::complex<double> (b1A - a1A * b2A);
        auto K1 = (B0 + B1 * s1) / (s1 - s2);
        auto K2 = -(B0 + B1 * s2) / (s1 - s2);

        auto e1 = std::exp (s1 * std::complex<double> (Ts));
        auto e2 = std::exp (s2 * std::complex<double> (Ts));
        auto e12 = std::exp ((s1 + s2) * std::complex<double> (Ts));
        auto CR = std::complex<double> (Ts / 2) * (K1 + K2);
        
        a1 = (float) -std::real (e1 + e2); a2 = (float)std::real (e12);

        b0 = (float)std::real (std::complex<double> (Ts) * (K1 + K2 + std::complex<double> (b2A)) - CR);
        b1 = (float)std::real (-std::complex<double> (Ts) * (K1 * e2 + K2 * e1 - std::complex<double> (b2A) * (e1 + e2)) - CR * std::complex<double> (a1));
        b2 = (float)std::real (std::complex<double> (Ts * b2A) * e12 - CR * std::complex<double> (a2));

        b3 = 0.f; b4 = 0.f;
        a3 = 0.f; a4 = 0.f;

        order = 2;
    }
    else if (algorithm == Algorithm_Massberg)
    {
        auto Wn = w0 * Ts;
        auto Q2 = Q * Q;

        auto prec = 1e-6;
        auto g1 = 2 / std::sqrt (std::pow (2 - std::pow (std::sqrt (2)*double_Pi / Wn, 2), 2) + std::pow (2 * double_Pi / (Q*Wn), 2));

        if (std::abs (1 - g1) < prec)
            g1 = g1 - prec;

        double Ws = 0.0;

        if (Q > sqrt (0.5))
        {
            auto gr = 2 * Q2 / sqrt (4 * Q2 - 1);
            auto Wr = jmax (jmin (std::abs (std::tan ((double) Wn*std::sqrt (1 - 1 / (2 * Q2)) / 2)), 1 / prec), prec);
            
            Ws = Wr * std::sqrt (std::sqrt ((gr * gr - g1 * g1) / (gr * gr - 1)));
        }
            
        else
        {
            Ws = jmin (std::tan (Wn * std::sqrt ((2 - 1 / Q2 + std::sqrt ((1 - 4 * Q2) / (Q2 * Q2) + 4 / g1)) / 2) / 2), 
                Wn * std::sqrt (std::sqrt (1 - g1 * g1)) / 2);
        }
            
        auto wp = 2 * std::atan (Ws);
        auto gp = 1 / std::sqrt (std::pow (1 - std::pow (wp / Wn, 2), 2) + std::pow (wp / (Q * Wn), 2));

        auto wz = 2 * std::atan (Ws / std::sqrt (g1));
        auto gz = 1 / std::sqrt (std::pow (1.0 - std::pow (wz / Wn, 2), 2) + std::pow (wz / (Q*Wn), 2));
        auto qp = std::sqrt (g1 * (gp * gp - gz * gz) / ((g1 + gz * gz)*std::pow (g1 - 1, 2)));
        auto qz = std::sqrt (g1 * g1 * (gp * gp - gz * gz) / (gz * gz * (g1 + gp * gp)*std::pow (g1 - 1, 2.0)));

        auto Ws2 = Ws * Ws;
        auto a0 = (float) (Ws2 + Ws / qp + 1);
        
        b0 = (float)(Ws2 + std::sqrt (g1)*Ws / qz + g1) / a0;
        b1 = (float)(2 * (Ws2 - g1)) / a0;
        b2 = (float)(Ws2 - std::sqrt (g1)*Ws / qz + g1) / a0;
                
        a1 = (float)(2 * (Ws2 - 1)) / a0;
        a2 = (float)(Ws2 - Ws / qp + 1) / a0;
                    
        b3 = 0.f; b4 = 0.f;
        a3 = 0.f; a4 = 0.f;

        order = 2;
    }
    else if (algorithm == Algorithm_Vicanek)
    {
        float b0A, b1A, b2A, a0A, a1A, a2A;
        SessionTwoDSP::calculusAnalogCoefficients (frequency, Q, 0.f, SessionTwoDSP::FilterType::Type_LowPass2,
            b0A, b1A, b2A, a1A, a2A);

        a0A = 1.f;
        b0A /= a2A; b1A /= a2A; b2A /= a2A;
        a0A /= a2A; a1A /= a2A; a2A /= a2A;

        std::complex<double> delta = a1A * a1A - 4 * a0A * a2A;
        std::complex<double> s1 = (std::complex<double> (-a1A) - std::sqrt (delta)) / std::complex<double> (2 * a2A);
        std::complex<double> s2 = (std::complex<double> (-a1A) + std::sqrt (delta)) / std::complex<double> (2 * a2A);
        
        auto e1 = std::exp (s1 * std::complex<double> (Ts));
        auto e2 = std::exp (s2 * std::complex<double> (Ts));
        auto e12 = std::exp ((s1 + s2) * std::complex<double> (Ts));
        auto a1d = -std::real (e1 + e2);
        auto a2d = std::real (e12);

        a1 = (float)a1d; 
        a2 = (float)a2d;

        auto A0 = std::pow (1 + a1d + a2d, 2);
        auto A1 = std::pow (1 - a1d + a2d, 2);
        auto A2 = -4 * a2;

        auto phi0 = 1 - std::pow (std::sin (w0 / 2 * Ts), 2);
        auto phi1 = std::pow (std::sin (w0 / 2 * Ts), 2);
        auto phi2 = 4 * phi0 * phi1;

        auto R1 = Q * Q * (A0 * phi0 + A1 * phi1 + A2 * phi2);
        auto B0 = A0;
        auto B1 = jmax (0.0, (R1 - B0 * phi0) / phi1);
                
        b0 = 0.5f * (float)(std::sqrt (B0) + std::sqrt (B1));
        b1 = (float)(std::sqrt (B0)) - b0;
        b2 = 0.f;

        b3 = 0.f; b4 = 0.f;
        a3 = 0.f; a4 = 0.f;

        order = 2;
    }
    else
    {
        b0 = 1.f; b1 = 0.f; b2 = 0.f; b3 = 0.f; b4 = 0.f;
        a1 = 0.f; a2 = 0.f; a3 = 0.f; a4 = 0.f;

        order = 0;
    }
}

const double SessionThreeDSP::getMagnitudeForFrequency (double frequency, double sampleRate,
    const float b0, const float b1, const float b2, const float b3, const float b4,
    const float a1, const float a2, const float a3, const float a4)
{
    constexpr std::complex<double> j (0, 1);

    jassert (frequency >= 0 && frequency <= sampleRate * 0.5);

    std::complex<double> numerator = 0.0, denominator = 0.0, factor = 1.0;
    std::complex<double> expjwT = std::exp (-MathConstants<double>::twoPi * frequency * j / sampleRate);

    numerator += ((double)b0) * factor; factor *= expjwT;
    numerator += ((double)b1) * factor; factor *= expjwT;
    numerator += ((double)b2) * factor; factor *= expjwT;
    numerator += ((double)b3) * factor; factor *= expjwT;
    numerator += ((double)b4) * factor;

    denominator = 1.0;
    factor = expjwT;

    denominator += ((double)a1) * factor; factor *= expjwT;
    denominator += ((double)a2) * factor; factor *= expjwT;
    denominator += ((double)a3) * factor; factor *= expjwT;
    denominator += ((double)a4) * factor;

    return std::abs (numerator / denominator);
}

const double SessionThreeDSP::getPhaseForFrequency (double frequency, double sampleRate,
    const float b0, const float b1, const float b2, const float b3, const float b4,
    const float a1, const float a2, const float a3, const float a4)
{
    constexpr std::complex<double> j (0, 1);

    jassert (frequency >= 0 && frequency <= sampleRate * 0.5);

    std::complex<double> numerator = 0.0, denominator = 0.0, factor = 1.0;
    std::complex<double> expjwT = std::exp (-MathConstants<double>::twoPi * frequency * j / sampleRate);

    numerator += ((double)b0) * factor; factor *= expjwT;
    numerator += ((double)b1) * factor; factor *= expjwT;
    numerator += ((double)b2) * factor; factor *= expjwT;
    numerator += ((double)b3) * factor; factor *= expjwT;
    numerator += ((double)b4) * factor;

    denominator = 1.0;
    factor = expjwT;

    denominator += ((double)a1) * factor; factor *= expjwT;
    denominator += ((double)a2) * factor; factor *= expjwT;
    denominator += ((double)a3) * factor; factor *= expjwT;
    denominator += ((double)a4) * factor;

    return std::arg (numerator / denominator);
}
