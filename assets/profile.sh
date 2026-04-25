# Environment
export PATH="/bin:/sbin:/usr/bin:/usr/sbin"

export PS1='\[\033[1;32m\][\u@\H]\[\033[0m\] \[\033[34m\]\w\[\033[0m\] \[\033[35m\]$\[\033[0m\] '

export EDITOR='/bin/vi'

# Source configuration files from /etc/profile.d
for i in /etc/profile.d/*.sh ; do
	if [ -r "$i" ]; then
		. $i
	fi
done
unset i

# Aliases
alias l='ls'
alias ll='ls -la'

alias v='vi'
alias md='mkdir'

vp () {
	v /etc/profile
}

vps () {
	vp && source /etc/profile
}

mcd () {
	mkdir $0
  cd $0
}
