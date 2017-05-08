struct temp {
	int x[3];
	int y;
};

float factorial (float n) {
	if(n==0) {
		return 1;
	}
	else {
		return n*factorial(n-1);
	}
}

struct temp g(int a, int b, int c){
	struct temp f;
	a=1101;
	b=2345;
	b=3456;
	f.x[0]=a;
	f.x[1]=b;
	f.x[2]=c;
	return f;
}

int try(int x[12],int y[12]) {
	int i;
	for(i=0; i<12; i++) {
		x[i]=i;
		printf(x[i]," ");
	}
	g(1,2,3);
	printf("\nx done\n");
	g(1,2,3);
	for(i=0; i<12; i++) {
		y[i]=i+1;
		printf(y[i]," ");
	}
	printf("\nydone\n");
	
	g(1,2,3);
	return 1;
}

int main () {
	int a,b;
	float blabal;
	int lol[12];
	int i;
	struct temp rand;
	a=4.5+9.5;
	for(i=0; i<12; i++) {
		lol[i]=1;
	}
	g(1,2,3);
	try(lol,lol);
	g(1,2,3);
	for(i=0; i<12; i++) {
		printf(lol[i]," ");
	}
	if(5.5+99){
		printf("\n",factorial(4.0),"\n");
	} else {
		printf("\n",factorial(7.0),"\n");
	}
}