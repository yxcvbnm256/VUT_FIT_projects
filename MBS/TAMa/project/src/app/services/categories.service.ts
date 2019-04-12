import { Injectable } from '@angular/core';

@Injectable({
  providedIn: 'root'
})
export class CategoriesService {
  categories = {
    Logistics: 'drive_eta',
    Pets: 'pets',
    'House Works': 'home',
    'Finance & Accounting': 'attach_money',
    IT: 'personal_video',
    Construction: 'build',
    'Teaching & Education': 'school',
    'Health Care': 'favorite',
    'Companions (18+)': 'face',
    Other: 'blur_on',
  };
}
