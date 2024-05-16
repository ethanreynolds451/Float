bool Float::limitEmpty(){
   return ! digitalRead(limitEmptyPin);
}

bool Float::limitFull(){
  return ! digitalRead(limitFullPin);
}
