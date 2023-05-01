import { Injectable } from '@angular/core';
import { User } from '../entities/user';
import { Observable } from 'rxjs';
import { HttpClient } from '@angular/common/http';

@Injectable({
  providedIn: 'root'
})
export class RegistrationService {
  baseUrl="";

  // constructor(private httpClient: HttpClient) { 
  // }

  // registerUser(user: User): Observable<Object>{
  //   return this.httpClient.post(`${this.baseUrl}`, user);
  // }
}
