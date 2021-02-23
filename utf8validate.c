#include <stdio.h>

int main()
{
	int ch;
	unsigned int ascii=0,utf=0,flag,codepoint,stop=0;
	
	/*
		ch = the character that we read with getchar() (1 byte each time)
		ascii & utf = counts how many ascii and utf8 characters we find
		codepoint = the number without the format (exmpl : ch = 0xxxxxxx -> codepoint = xxxxxxx etc)
		flag = turns 1 when ch doesnt fit any header format
		stop = a flag which indicates if there has been any errors. if stop=1 then we have found an error so we print ONLY the error.
	*/

	while ((ch=getchar()) != EOF)
	{

		flag=0;

		if ((ch & 128) == 0)        																	    // ASCII
		{
			flag=1;
			codepoint = (unsigned int)ch;
			++ascii;
		}

		else                              															    	// multi-byte UTF-8

		{
			flag = 0;
			if ((ch & 224) == 192)           														       // !!! FOR 2 BYTES !!! | 1st byte | 224 = 11100000 | 192 = 11000000
			{
				flag=1;
				codepoint = (unsigned int)ch;
				codepoint &= 31	;																		   // 31 = 00011111
				if ((ch=getchar()) != EOF)   															   // 2nd byte
				{
					if ((ch & 192) == 128)                  											   // 2nd byte | 192 = 11000000 | 128 = 10000000
					{
						codepoint <<= 6;                   												   // shift 6 times
						codepoint |= (ch & 63);             											   // 63 = 00111111
						if (codepoint <= 127)                											   // 127 = max ascii characters (01111111)
						{
							printf ("Oversized UTF-8 code point: U+%04X\n",codepoint);
							stop=1;
							break;
						}
						else
						{
							++utf;
						}
					}
					else
					{
						printf ("Invalid UTF-8 tail byte: 0x%04X\n",ch);
						stop=1;
						break;
					}
				}
			}

			else if ((ch & 240) == 224)                														 // !!! FOR 3 BYTES !!! | 1st byte | 240 = 11110000 | 224 = 11100000
			{
				flag=1;
				codepoint = (unsigned int)ch;
				codepoint &= 15  ;																			// 15 = 00001111
				if ((ch=getchar()) != EOF)
				{
					if ((ch & 192) == 128)																	// 2nd byte | 192 = 11000000 | 128 = 10000000
					{
						codepoint <<= 6;
						codepoint |= (ch & 63);																// 63 = 00111111
						if ((ch=getchar()) != EOF)
						{
							if ((ch & 192) == 128)															// 3rd byte | 192 = 11000000 | 128 = 10000000
							{
								codepoint <<= 6;
								codepoint |= (ch & 63);        												// 63 = 00111111
								if (codepoint <= 2047)														// OVERSIZED
								{
									printf ("Oversized UTF-8 code point: U+%04X\n",codepoint);
									stop=1;
									break;
								}
								else if ((codepoint >= 55296) && (codepoint <= 57343))						// INVALID CODEPOINT
								{
									printf ("Invalid UTF-8 code point: U+%04X\n",codepoint);
									stop=1;
									break;
								}
								else
								{
									++utf;
								}
							}
							else
							{
								printf ("Invalid UTF-8 tail byte: 0x%04X\n",ch);
								stop=1;
								break;
							}
						}
					}
					else
					{
						printf ("Invalid UTF-8 tail byte: 0x%04X\n",ch);
						stop=1;
						break;
					}
				}
			}

			else if ((ch & 248) == 240)               																		// !!! FOR 4 BYTES !!! | 1st byte | 248 = 11111000 | 240 = 11110000
			{
				flag=1;
				codepoint = (unsigned int)ch;
				codepoint &= 7	;																							// 7 = 00000111
				if ((ch=getchar()) != EOF)
				{
					if ((ch & 192) == 128)																					// 2nd byte | 192 = 11000000 | 128 = 10000000
					{
						codepoint <<= 6;
						codepoint |= (ch & 63);																				// 63 = 00111111
						if ((ch=getchar()) != EOF)
						{
							if ((ch & 192) == 128)																			// 192 = 11000000 | 128 = 10000000
							{
								codepoint <<= 6;
								codepoint |= (ch & 63);																		// 63 = 00111111
								if ((ch=getchar()) != EOF)
								{
									if ((ch & 192) == 128)																	// 192 = 11000000 | 128 = 10000000
									{
										codepoint <<= 6;
										codepoint |= (ch & 63);																// 63 = 00111111 
										if (((codepoint >= 55296) && (codepoint <= 57343)) || (codepoint > 1114111))		// INVALID CODEPOINT | (U+D7FF , U+E000)
										{
											printf ("Invalid UTF-8 code point: U+%04X\n",codepoint);
											stop=1;
											break;
										}
										else if (codepoint <= 65535)														// OVERSIZED
										{
											printf ("Oversized UTF-8 code point: U+%04X\n",codepoint);
											stop=1;
											break;
										}
										else
										{
											++utf;
										}
									}
									else
									{
										printf ("Invalid UTF-8 tail byte: 0x%04X\n",ch);
										stop=1;
										break;
									}
								}
							}
							else
							{
								printf ("Invalid UTF-8 tail byte: 0x%04X\n",ch);
								stop=1;
								break;
							}
						}
					}
					else
					{
						printf ("Invalid UTF-8 tail byte: 0x%04X\n",ch);
						stop=1;
						break;
					}
				}
			}
		}


		if (flag==0) 																						// Header byte doesn't fit any format
		{
			printf ("Invalid UTF-8 header byte: 0x%04X\n",ch);
			stop=1;
			break;
		}


	}

	if (stop==0)																							// stop = 0 , means that there are no errors
	{
		printf ("Found %d ASCII and %d multi-byte UTF-8 characters\n",ascii,utf);
	}

}
