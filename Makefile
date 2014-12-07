CPPFLAGS = -Wall -g
LDFLAGS = -lpqxx
OBJDIR = obj/
SRCDIR = src/
INCDIRS = include/
INC = $(foreach d, $(INCDIRS), -I$d)
OUTPUT = prva
COMPILER = g++

RM = rm -rf

SRCS = $(shell find $(SRCDIR) -name "*.cpp")
OBJS = $(addprefix $(OBJDIR),$(subst $(SRCDIR),,$(SRCS:.cpp=.o)))

all: psql

psql: $(OBJS)
	$(COMPILER) $(OBJS) $(LDFLAGS) -o $(OUTPUT)

$(OBJDIR)%.o: $(SRCDIR)%.cpp
	@mkdir -p $(@D)
	$(COMPILER) $(CPPFLAGS) $(INC) -c $< -o $@

clean:
	$(RM) $(OBJDIR)* $(OUTPUT)

clear: clean psql
