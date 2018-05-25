%	Author:		Tomas Zubrik xzubri00@stud.fit.vutbr.cz
%	Projekt:	ISS Project, VUT Brno 2017
%	Resources:	https://www.fit.vutbr.cz/study/courses/ISS/public/ --- study phase
%				https://www.mathworks.com/matlabcentral/answers/215704-xcorr-how-to-find-the-location-of-the-highest-correlation-on-the-actual-data-using-xcorr
%				https://www.mathworks.com/help/signal/ref/square.html?s_tid=srchtitle

% Task 1
	wavfile = 'xzubri00.wav';
	[x, Fs] = audioread(wavfile); x=x'; 	%x - loaded signal, Fs - sampling freq	
	N = length(x); 							%samples count
	time = N/Fs;							%time in seconds
	
% Task 2
	f = (0:N/2-1) / N * Fs; %freq axis

    G = 10 * log10((abs(fft(x)).^2)/N); %power spectral density
	G = G(1:N/2); 	%from 1 to 8000
    figure; plot (f,G); title('Power Spectral Density'); xlabel('f [Hz]'); ylabel('PSD [dB]'); grid on; 
			
    mag = abs(fft(x));  %magnitude
	mag = mag(1:N/2); 	%from 1 to 8000
    figure; plot (f,mag); title('Magnitude of Spectrum'); xlabel('f [Hz]'); ylabel('|X(j\omega)|'); grid on; 
		
% Task 3
	magmax = f(find(G == max(G))); % finds maximum's position
		
% Task 4
	b = [0.2324 -0.4112 0.2324]; a = [1 0.2289 0.4662]; %b & a coeff.
	figure; zplane(b,a);		%zeros & poles plane
	if(abs(roots(a)) < 1 | isempty(roots(a))) Stable = true; else Stable = false; end

% Task 5
	n = 512; hFs = Fs/2; 
	f =(0:n-1) / n * hFs;
    H = abs(freqz(b, a, n)); %freq characteristics
    figure; plot(f, H); title('Frequency characteristics magnitude'); xlabel('f [Hz]'); ylabel('|H(f)|'); axis([0 hFs 0 1.2]); grid on; 

% Task 6
	N = length(x);
	f = (0:N/2-1)/N * Fs; 
	filx = filter(b, a, x); 	%filtered signal
	filx = filx(1:time * Fs);
	
	mag = abs(fft(filx)); %magnitude
	mag = mag(1:N/2);	
    figure; plot (f,mag); title('Magnitude of Spectrum (Filtered Signal)'); xlabel('f [Hz]'); ylabel ('|X(j\omega)|'); 	grid on;
	
    filG = 10 * log10((abs(fft(filx)).^2)/N); %power spectral density  
	filG = filG(1:N/2);   
    figure; plot (f,filG); title ('Power Spectral Density (Filtered Signal)'); xlabel('f [Hz]'); ylabel ('PSD [dB]'); grid on; 
		
% Task 7
    filmagmax = f(find(filG == max(filG)));  %finds maximum's position of magnitude of filtered signal
	
% Task 8 
	t = 1e-6:1/Fs:0.02; %nice squares thats why from 0.000001
	x2 = square(2*pi*t*4000);
	figure;	plot(t,x2);	title('Square Periodic Wave'); xlabel('Time [s]'); ylabel('Amplitude'); axis([0 0.020 -1.10 1.10]);
	[y, lags] = xcorr(x,x2);
	[~,index] = max(abs(y));
	index = lags(index)+1; %indexed from 1 and we have started from 0.0000001 thats why +1
	timepos = index/Fs;

% Task 9
	c = 50; 	%constant 50
	k = -c:c; 	%k = <-50, 50>
	Rv = xcorr(x,'biased');
	Rv = Rv(N-c:N+c);	%
	figure; plot(k, Rv); title ('Autocorrelation coefficients R[k]'); xlabel('k');

% Task 10
    R_10 = Rv(c+11); % Rv(index == 1 + 10) Rv is vector with 100 samples 

% Task 11
	xlin = linspace(min(x), max(x)); xlin = xlin'; 		%generated signal
	L = length(xlin); N = length(x); h = zeros(L,L); 	%initialized to zeros
	bigx = repmat(xlin,1,N); 
	bigy = repmat(x,L,1);
	[~,ind1] = min(abs(bigy - bigx));
	ind2 = ind1(11:N); %ind2 10 samples delayed
	for ii=1:N-10,
		d1 = ind1(ii);   d2 = ind2(ii); 
		h(d1,d2) = h(d1,d2) + 1; 
	end
	surf = (xlin(2) - xlin(1))^2; % surface of one tile
	p = h / N / surf;  
	figure; bar3(p);  title('3D model of combined probability distribution density function'); xlabel('x1'); ylabel('x2'); axis([0 101 0 101 0 3]);

% Task 12
    check = sum(sum(p)) * surf;
    disp(['hist2: check -- 2d integral should be 1 and is ' num2str(check)]);

% Task 13
    xlin = xlin(:); X1 = repmat(xlin, 1, L);
    xlin = xlin'; X2 = repmat(xlin, L, 1);
    R_10_est = sum(sum(X1 .* X2 .* p)) * surf;
