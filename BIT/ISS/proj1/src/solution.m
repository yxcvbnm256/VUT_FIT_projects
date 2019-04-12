% 1. ukol
I = imread('http://www.fit.vutbr.cz/study/courses/ISS/public/proj2014-15/obrazek_pod_stromecek/xpawlu00.bmp')
H = [-0.5,-0.5,-0.5;-0.5,5,-0.5;-0.5,-0.5,-0.5]
step1 = imfilter(I, H)
imwrite(step1, 'step1.bmp')

% 2. ukol
step2 = flipdim(step1, 2)
imwrite(step2, 'step2.bmp')

% 3. ukol
step3 = medfilt2(step2, [5,5])
imwrite(step3, 'step3.bmp')

% 4. ukol
H = [1,1,1,1,1;1,3,3,3,1;1,3,9,3,1;1,3,3,3,1;1,1,1,1,1]/49
step4 = imfilter(step3, H)
imwrite(step4, 'step4.bmp')

% 5. ukol
step4double = double(step4)
step4double = flipdim(step4double,2)
Idouble = double(I)
noise = 0
for (y=1:512)
	for (x=1:512)
		noise=noise+(abs(Idouble(x,y)-step4double(x,y)));
	end;
end;
noise=noise/(512*512)

% 6. ukol
step4double = im2double(step4)
minIM=min(min(step4double));
maxIM=max(max(step4double));

step5double=imadjust(step4double, [minIM, maxIM]);
step5=im2uint8(step5double);
imwrite(step5, 'step5.bmp')

% 7. ukol
step4ddouble = double(step4);
step5ddouble = double(step5);
mean_no_hist = mean(step4ddouble(:));
std_no_hist = std(step4ddouble(:));
mean_hist = mean(step5ddouble(:));
std_hist = std(step5ddouble(:));

% 8. ukol
step5double = double(step5)
x = size(step5double);
N = 2;
step6double = zeros(x(1),x(2));
for k=1:x(1)
  for l=1:x(2)
    step6double(k,l) = round(((2^2)-1)*(step5double(k,l)-0)/(255))*(255)/((2^2)-1);
  end
end
step6=uint8(step6double);
imwrite(step6, 'step6.bmp');
