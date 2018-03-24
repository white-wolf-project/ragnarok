##
# @file test.py
# @brief Simple file called with the python loader
# Also used as an example for Doxygen

## Function : test_funct
#
#  This function prints test like this :
# @code
# def test_funct(something):
# 	print("%s" % someting)
# @endcode
# Then call it like this :
# @verbatim test_funct("this is a simple test") @endverbatim
# @param something : variable used to set something to print
# You can do the same for Python classes
def test_funct(someting):
	print("%s" % someting)

# call test function
test_funct("this is a simple test")