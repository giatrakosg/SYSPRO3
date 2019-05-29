# SYSPRO3
# Λεπτομερειες
1. Ο server δεχεται εντολες 16 byte και ip long (8 byte) , port short(2 byte) η διαφορα
καλυπτεται με spaces .
2. Το μυνημα GET_CLIENTS στελνεται πανω στη ιδια συνδεση με το LOG_ON οπως και η απαντηση .**Υποθετουμε οτι ο client θα στειλει GET_CLIENTS** .
# Πηγες
* Εχει χρησιμοποιηθει κωδικας απο το https://www.ibm.com/support/knowledgecenter/en/ssw_ibm_i_74/rzab6/poll.htm
 ως σκελετος για ενα server με poll.
* Εχει χρηιμοποιηθει κωδικας απο το https://www.geeksforgeeks.org/c-program-display-hostname-ip-address/ για την ευρεση της τοπικης ip .
