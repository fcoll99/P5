fs=16000;
t=0: (1/fs) :0.20;

N1=30;
N2=2;
I=1;

fc=300;
fm=(N2/N1)*fc;

x=0.5*sin(2*pi*fc*t + I*sin(2*pi*fm*t));
y=0.5*sin(2*pi*fm*t);

plot(t,x);
hold on
plot(t,y);
legend("Señal completa con fc=300Hz","Señal vibrato con fm=20Hz");
ylim([-1,1]);