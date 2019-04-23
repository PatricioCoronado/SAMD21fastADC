# SAMD21fastADC
Librería para configurar el ADC de Arduino Zero y compatibles; boards con SAMD21 
Arduino-Zero-SAMD21-ADC-library
Arduino Zero (SAMD21) ADC library for fast and noise free analog conversion

Establishes 3 fundamental parameters for the acquisition speed.

---Prescaler---- 0 =< Prescaler =< 7

To get CLK_ADC divide 48MHz by 2^(Prescaler+2)

---SamplenNum--- 0 =< SamplenNum =< 8

number of samples averaged in each acquisition samples averaged = 2^SamplenNum

---Samplen--- 0 =< Samplen =< 63

It determines the number of cycles during which the input pin is read. So set time during which the input pin is read = "Sampling time"

Sampling time = (Samplen+1).(CYCLE_CLK/2). CYCLE_CLK depends on Prescaler.

Why do this? The analog input of the ADC has a capacity of 3.3pF and a series resistance of 3.3Koh. To do a good measure you need to charge that capacity. The time required for that depends on the source output resistance. If it is high you need more clk cycles to load the input capacity. t_charge=5(3.3e3 + Rs)3.3e-12 (charging 99%) And the time of a CLK_ADC cycle is T_CLK_ADC = Prescaler/(48e6) Are needed t_charge/T_CLK cyckes

Examples: The default Arduino platform configuration is:

configure_ADC(7,0,63); //Conversion time = 430 microseconds

Faster configuration is: (Average 8 samples)

configure_ADC(3,3,5); //Conversion time = 61 microseconds averages 8 samples Fasten configuration that works fine is:

configure_ADC(3,0,5); //Conversion time = 15 microseconds

The function returns: 0 no errors: -1 Prescaler out of range -2 SamplenNum out of range -3 Samplen out of range
