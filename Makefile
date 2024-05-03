SRCS	=	$(addprefix srcs/,\
			main.cpp\
			ServerConfig.cpp\
			server_init.cpp \
			ServerLocation.cpp\
			load_config.cpp\
			Response.cpp\
			AHTTP.cpp\
			Request.cpp\
			process_request.cpp\
			cgi.cpp\
			Get.cpp\
			Post.cpp \
			Delete.cpp \
			)

OBJS	=	$(SRCS:srcs/%.cpp=objs/%.o)

DEPS	=	$(OBJS:.o=.d)

NAME	=	webserv

FLAGS	=	-Wall -Wextra -Werror -std=c++98 -g3

CC		=	c++

all		:	${NAME}

${OBJS}	:	objs/%.o: srcs/%.cpp | objs
	${CC} -I incs ${FLAGS} -MMD -c $< -o $@

${NAME}	:	${OBJS}
	${CC} ${FLAGS} ${OBJS} -o ${NAME}

clean	:
	rm -rf objs

fclean	:	clean
	rm -f ${NAME}

re		:	fclean all

objs 	:
	mkdir $@

-include	${DEPS}

.PHONY	:	all clean fclean re
