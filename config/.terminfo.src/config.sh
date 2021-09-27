#Add the terminfo to .terminfo and /etc/terminfo:
rm -fr $HOME/.terminfo/s $HOME/.terminfo/t $HOME/.terminfo/x

for i in $HOME/.terminfo.src/*.terminfo ; do \
        tic -xo $HOME/.terminfo $i ; 
        tic -x $i ;  done

