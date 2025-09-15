NAME = pie_chart

CTAGS=ctags

OBJS_FOLDER = obj
INCS_FOLDER = ./
SRCS_FOLDER = ./

DB_SRCS_FOLDER = $(SRCS_FOLDER)/database
#GUI_SRCS_FOLDER = $(SRCS_FOLDER)/gui
INCS := $(wildcard $(INCS_FOLDER)/*.h)

DB_SRC = $(DB_SRCS_FOLDER)/my_sql.c

SRCS := $(notdir $(wildcard $(SRCS_FOLDER)/*.c $(DB_SRC) $(GUI_SRCS_FOLDER)/*.c))

OBJS := $(patsubst %.c, $(OBJS_FOLDER)/%.o, $(SRCS))

CC = gcc

CFLAGS = -Wall -g -Wno-deprecated-declarations

GTK_FLAGS = `pkg-config --cflags gtk+-3.0`
GTK_LIBS_FLAGS = `pkg-config --libs gtk+-3.0`

LIB_FLAGS = -lm -lsqlite3

.PHONY: all tags install clean uninstall reinstall

all: install

install: $(NAME)

$(NAME): $(OBJS_FOLDER) $(OBJS)
	echo " LINK object files "
	$(CC) $(CFLAGS) $(OBJS) $(LIB_FLAGS) $(GTK_LIBS_FLAGS) -o $@

$(OBJS_FOLDER):
	mkdir $(OBJS_FOLDER)

$(OBJS_FOLDER)/%.o:$(SRCS_FOLDER)/%.c $(INCS)
	@$(CC) $(CFLAGS) $(GTK_FLAGS) \
					-I $(INCS_FOLDER) \
					-c $< -o $@

$(OBJS_FOLDER)/%.o:$(DB_SRCS_FOLDER)/%.c $(INCS)
	@$(CC) $(CFLAGS) $(GTK_FLAGS) \
					-I $(INCS_FOLDER) \
					-c $< -o $@

$(OBJS_FOLDER)/%.o:$(GUI_SRCS_FOLDER)/%.c $(INCS)
	@$(CC) $(CFLAGS) $(GTK_FLAGS) \
					-I $(INCS_FOLDER) \
					-c $< -o $@

uninstall: clean
	rm -f $(OBJS_FOLDER)/*.o

FORCE: ;

tags: FORCE
	${RM} -f tags
	${CTAGS} -R $(SRCS_FOLDER)

clean:
	rm -f $(NAME)
	rm -rf $(OBJS_FOLDER)

reinstall: uninstall all

buildrepo:
	$(info -->buildrepo: objdir=$(OBJDIR) srcdir=$(SRCDIRS)  )
	@$(call make-repo)

# Create obj directory structure
define make-repo
	mkdir -p $(OBJDIR)
		for dir in $(SRCDIRS); \
		do \
			mkdir -p $(OBJDIR)/$$dir; \
		done
endef
