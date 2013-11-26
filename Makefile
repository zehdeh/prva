CPPFLAGS = -Wall -g
LDFLAGS = -lpq
OBJDIR = obj/
SRCDIR = src/
INCDIRS = include/ /usr/include/postgresql/
INC = $(foreach d, $(INCDIRS), -I$d)
OUTPUT = test

RM = rm -rf

SRCS = $(shell find $(SRCDIR) -name "*.cpp")
OBJS = $(addprefix $(OBJDIR),$(subst $(SRCDIR),,$(SRCS:.cpp=.o)))

all: psql

psql: $(OBJS)
	g++ $(OBJS) $(LDFLAGS) -o $(OUTPUT)

$(OBJDIR)%.o: $(SRCDIR)%.cpp
	@mkdir -p $(@D)
	g++ $(CPPFLAGS) $(INC) -c $< -o $@

clean:
	$(RM) $(OBJDIR)* $(OUTPUT)

clear: clean psql
