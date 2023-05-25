import { Injectable } from '@angular/core';
import { User } from '../entities/user';
import { Observable } from 'rxjs';
import { HttpClient } from '@angular/common/http';

@Injectable({
  providedIn: 'root'
})
export class LoginService {
  private baseUrl="http://localhost:8083/login";
  private loggedIn = false;

  constructor(private httpClient: HttpClient ) { }

  loginUser(user: User): Observable<object> {
    return this.httpClient.post(`${this.baseUrl}`, user);
  }

  isLoggedIn(): boolean {
    return this.loggedIn;
  }

  setLoggedIn(value: boolean): void {
    this.loggedIn = value;
  }

  logout(): void {
    this.loggedIn = false;
  }
}
