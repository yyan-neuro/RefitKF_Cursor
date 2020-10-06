global A W C Q Xt_;

% READ AND PARSE THE DATA

fid = fopen('C:\BCIPrograms\SDL_2D_Cursor\SDL_2D_Cursor\record_con.txt');
movem = [];
target = [0,0];
features = importdata('D:\Data\features.txt');
features = features.';

tline = fgetl(fid);
linenum = 1;
waitIndex = [];


%maximum velocity and contact radius
radd = 41;
maxv = 5;

%temp vars
vxn = 0;
vyn = 0;
base = 0;


while ischar(tline)
    if(strcmp(tline(1),'T'))
        temp = tline(8:end);
        target=  sscanf(temp,'%f');
    elseif(strcmp(tline(1),'W'))
        waitIndex = [waitIndex,linenum];
        linenum = linenum + 1;
    elseif(strcmp(tline(1),'S') || strcmp(tline(1),'F') || strcmp(tline(1), 'M'))
        %ignore the success or failure tag
        
    else
        movevec = sscanf(tline,'%f');
        movevec = [movevec;0;0];
        tvx = target(1)-movevec(1);
        tvy = target(2)-movevec(2);
        
        
        %normalize the velocity
        radius = sqrt(tvx*tvx+tvy*tvy);
        if(radius<radd)
            vxn = 0;
            vyn = 0;
        else   
            base = maxv / radius;
            vxn = tvx*base;
            vyn = tvy*base;
        end
        %add to movement vector
        movevec(3) = vxn;
        movevec(4) = vyn;
        
          
        
        
        movevec(5) = 1;
        movem = [movem,movevec];
        linenum = linenum + 1;
    end    
    tline = fgetl(fid);

end

fclose(fid);
h = msgbox('Read features / movement Completed');
features(:,waitIndex) = [];



%KALMAN FILTER TRAINING
cutoff = 1000;
features = features(:,end-cutoff:end);
movem = movem(:,end-cutoff:end);

lr = 0.5;
aratio = importdata('C:\BCIPrograms\SDL_2D_Cursor\SDL_2D_Cursor\aratio.txt');


if(aratio==0)
    [A,W,C,Q] = Refit_KF_Train_Constrain(features,movem,0.1);
else
    [A2,W2,C2,Q2] = Refit_KF_Train_Constrain(features,movem,0.1);
    A = (1-lr)*A + lr*A2;
    W = (1-lr)*W + lr*W2;
    C = (1-lr)*C + lr*C2;
    Q = (1-lr)*Q + lr*Q2;
end

aratio = min(aratio,1);


mark = fopen('C:\BCIPrograms\SDL_2D_Cursor\SDL_2D_Cursor\record_con.txt','a');
fprintf(mark,'%s\n','M');
fclose(mark);

Xt_(1) = 480; Xt_(2) = 300;

h = msgbox('Retraining Completed');




