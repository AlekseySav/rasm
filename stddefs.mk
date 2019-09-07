_DEBUG	= -g	# uncoment this to make debug

AR 		= ar
LD		= ld $(_DEBUG)
CC		= gcc $(_DEBUG)
CFLAG 	= -c -I $(C_INC)

%.o: %.c
	$(CC) $(CFLAG) $< -o $@
 