import { Injectable } from '@angular/core';
import { User } from '../entities/user';
import { Observable } from 'rxjs';
import { HttpClient } from '@angular/common/http';

@Injectable({
  providedIn: 'root'
})
export class LoginService {
  private baseUrl="http://localhost:8083/login";

  constructor(private httpClient: HttpClient ) { }

  loginUser(user: User): Observable<object> {
    return this.httpClient.post(`${this.baseUrl}`, user);
  }
}
